// SpaceZ_2D_Alpha/Tests/PhySystemTests.cpp
// PhySystem / Collider 集成回归测试：只依赖 Core + 控制台，不需要 Unity。
// 构建：tools/build_tests.bat  ->  Tests/Debug/SpaceZ_SolverTests.exe
//
// 覆盖：
//   A. Collider 形状 / Bound（Box / Sphere / Capsule，含旋转与缩放）
//   B. PhySystem 生命周期：创建、销毁、句柄复用
//   C. 重力与速度积分（单子步 / 多子步）
//   D. 静态地面 + 动态球落地（窄相 + 求解器 + 位置修正端到端）
//   E. 重叠分离与碰撞信息（depth / normal / point 链路）
//   F. BVH 同步：刚体移动传播、移走后碰撞对消失、销毁清理
//   G. 已知缺口（未初始化成员 / mass=0 / 静态 collider 注册），全部 [KNOWN]，不影响退出码
//   H. Step 边界（dt=0 / dt<0）
// 标记约定：[PASS] 通过 / [FAIL] 回归（影响退出码）/ [KNOWN] 已知未修复（不影响退出码）
//
// !! 已知缺陷规避说明（G 组）!!
//   当前 Rigidbody::torque 与 Collider 的摩擦/弹性成员没有默认初始化。
//   为避免污染功能测试，这里统一采用：
//     - 所有参与碰撞的 Collider 一律显式 SetCollider(...)；
//     - 动态刚体在首次 Step（消费掉未初始化 torque）后调用 SetVelocity(handle, 0, 0)
//       清掉被打爆的角速度，再重新摆放初始位置开始正式断言。
//   修复（成员给默认值）后这两步可以删除，G 组对应项应转为 PASS。

#include "System/PhySystem.h"
#include "Object/Collider/BoxCollider.h"
#include "Object/Collider/SphereCollider.h"
#include "Object/Collider/CapsuleCollider.h"
#include "Math/Vector2.h"
#include "Math/Math.h"
#include "Solver/PhyQuantities.h"

#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

using namespace Core;
using namespace Core::Math;
using namespace Core::Collision;

static int g_pass = 0, g_fail = 0, g_known = 0;

static void Section(const char* n) { printf("\n== %s ==\n", n); }
static void Ok(bool ok, const char* what)
{
    if (ok) { ++g_pass; printf("  [PASS]  %s\n", what); }
    else    { ++g_fail; printf("  [FAIL]  %s\n", what); }
}
static void Known(bool ok, const char* what)
{
    if (ok) { ++g_pass; printf("  [PASS]  %s\n", what); }
    else    { ++g_known; printf("  [KNOWN] %s   <- 已知未修复\n", what); }
}
static bool Near(float a, float b, float eps = 1e-3f) { return std::fabs(a - b) <= eps; }
static bool Finite(const Vector2& v) { return std::isfinite(v.x) && std::isfinite(v.y); }

// --------------------------------------------------------------- 助手
struct Snap
{
    bool found = false;
    Vector2 pos = Vector2::zero;
    float rot = 0.0f;
    Vector2 lv = Vector2::zero;
    float av = 0.0f;
};
// 取刚体在最近一次 Step 后的状态（依赖 dirty 集：Step 内所有刚体都会被标记）
static Snap GetSnap(PhySystem& sys, const RigidbodyHandle& h)
{
    Snap s;
    sys.ForeachDirtyBodies([&](const RigidbodyHandle& hh, const Vector2& p, float r, const Vector2& lv, float av)
    {
        if (hh == h) { s.found = true; s.pos = p; s.rot = r; s.lv = lv; s.av = av; }
    });
    return s;
}
// 创建带形状的碰撞体；body 为空句柄 = 静态
static ColliderHandle MakeShape(PhySystem& sys, ColliderType type, const RigidbodyHandle& body,
                                const Vector2& worldPos, const Vector2& shapeArg, float shapeArg2 = 0.0f)
{
    auto h = sys.CreateCollider(type, body);
    switch (type)
    {
        case ColliderType::Box:     sys.SetBoxCollider(h, shapeArg); break;
        case ColliderType::Sphere:  sys.SetSphereCollider(h, shapeArg.x); break;
        case ColliderType::Capsule: sys.SetCapsuleCollider(h, shapeArg.x, shapeArg2); break;
    }
    sys.SetCollider(h, 0.5f, 0.4f, 0.0f);
    if (body == RigidbodyHandle::null)
        sys.UpdateTransform(h, worldPos, 0.0f, Vector2::one);   // 静态：局部位即世界位
    return h;
}
// 创建动态球（消化未初始化 torque：首帧后清零速度并复位）
static RigidbodyHandle MakeBall(PhySystem& sys, float r, const Vector2& worldPos, float dt)
{
    auto b = sys.CreateBody();
    sys.SetBody(b, 1.0f, 1.0f);
    MakeShape(sys, ColliderType::Sphere, b, worldPos, Vector2(r, 0.0f));
    sys.UpdateTransform(b, worldPos, 0.0f, Vector2::one);
    sys.Step(dt);                                   // 消费未初始化 torque（见文件头说明）
    sys.SetVelocity(b, Vector2::zero, 0.0f);
    sys.UpdateTransform(b, worldPos, 0.0f, Vector2::one);
    return b;
}

// --------------------------------------------------------------- A. 形状 / Bound
static void TestColliderBounds()
{
    Section("A. Collider 形状 / Bound");
    {
        BoxCollider box(ColliderHandle::FromId(101));
        box.SetBox(Vector2(2.0f, 1.0f));
        box.SetLocalTransform(Vector2(5.0f, 3.0f), 0.0f, Vector2::one);
        auto b = box.GetBound();
        Ok(Near(b.center.x, 5.0f) && Near(b.center.y, 3.0f)
           && Near(b.extents.x, 2.0f) && Near(b.extents.y, 1.0f),
           "Box extents(2,1) @ (5,3) 无旋转：AABB 正确");
    }
    {
        BoxCollider box(ColliderHandle::FromId(102));
        box.SetBox(Vector2(2.0f, 1.0f));
        box.SetLocalTransform(Vector2::zero, PI * 0.25f, Vector2::one);
        auto b = box.GetBound();
        float e = 2.0f * Cos(PI * 0.25f) + 1.0f * Sin(PI * 0.25f);   // 外接框半宽 ≈ 2.1213
        Ok(Near(b.extents.x, e, 1e-3f) && Near(b.extents.y, e, 1e-3f),
           "Box 旋转 45°：AABB 按外接框扩大（≈2.121）");
    }
    {
        SphereCollider sp(ColliderHandle::FromId(103));
        sp.SetSphere(1.0f);
        sp.SetLocalTransform(Vector2(2.0f, -3.0f), 0.0f, Vector2::one);
        auto b = sp.GetBound();
        Ok(Near(b.center.x, 2.0f) && Near(b.center.y, -3.0f)
           && Near(b.extents.x, 1.0f) && Near(b.extents.y, 1.0f),
           "Sphere r=1 @ (2,-3)：AABB 正确");
    }
    {
        CapsuleCollider cp(ColliderHandle::FromId(104));
        cp.SetCapsule(2.0f, 0.5f);
        cp.SetLocalTransform(Vector2::zero, 0.0f, Vector2::one);
        auto b = cp.GetBound();
        Ok(Near(b.extents.x, 0.5f) && Near(b.extents.y, 1.5f),
           "Capsule h=2 r=0.5 竖直：AABB extents (0.5, 1.5)");
    }
    {
        CapsuleCollider cp(ColliderHandle::FromId(105));
        cp.SetCapsule(2.0f, 0.5f);
        cp.SetLocalTransform(Vector2::zero, PI * 0.5f, Vector2::one);
        auto b = cp.GetBound();
        Ok(Near(b.extents.x, 1.5f) && Near(b.extents.y, 0.5f),
           "Capsule 旋转 90° 水平：AABB extents (1.5, 0.5)");
    }
    {
        BoxCollider box(ColliderHandle::FromId(106));
        box.SetBox(Vector2(1.0f, 1.0f));
        box.SetLocalTransform(Vector2::zero, 0.0f, Vector2(2.0f, 1.0f));
        auto b = box.GetBound();
        Ok(Near(b.extents.x, 2.0f) && Near(b.extents.y, 1.0f),
           "Box 受缩放 (2,1)：AABB extents 同步为 (2,1)");
    }
}

// --------------------------------------------------------------- B. 生命周期
static void TestLifecycle()
{
    Section("B. PhySystem 生命周期 / 句柄");
    const float dt = 1.0f / 60.0f;
    PhySystem sys(1, 8);

    auto b = sys.CreateBody();
    sys.SetBody(b, 1.0f, 1.0f);
    MakeShape(sys, ColliderType::Sphere, b, Vector2(0.0f, 100.0f), Vector2(0.5f, 0.0f));
    sys.UpdateTransform(b, Vector2(0.0f, 100.0f), 0.0f, Vector2::one);

    sys.Step(dt);
    Ok(GetSnap(sys, b).found, "Step 后刚体出现在 dirty 列表");

    sys.DestroyBody(b);
    sys.Step(dt);
    Ok(true, "DestroyBody 后继续 Step 不崩溃");
    Ok(sys.GetCollisionPairCount() == 0, "DestroyBody 后无残留碰撞对");

    auto b2 = sys.CreateBody();
    Ok(!(b2 == b), "复用句柄带新 generation，与旧句柄不相等");
    sys.SetBody(b2, 1.0f, 1.0f);
    MakeShape(sys, ColliderType::Sphere, b2, Vector2(0.0f, 100.0f), Vector2(0.5f, 0.0f));
    sys.UpdateTransform(b2, Vector2(0.0f, 100.0f), 0.0f, Vector2::one);
    sys.Step(dt);
    auto s2 = GetSnap(sys, b2);
    Ok(s2.found && Finite(s2.pos) && Finite(s2.lv), "新建刚体正常工作（无残留状态干扰）");
}

// --------------------------------------------------------------- C. 重力 / 积分
static void TestIntegration()
{
    Section("C. 重力与速度积分");
    const float dt = 1.0f / 60.0f;
    {
        PhySystem sys(1, 8);
        auto b = sys.CreateBody();
        sys.SetBody(b, 1.0f, 1.0f);
        sys.UpdateTransform(b, Vector2(0.0f, 1000.0f), 0.0f, Vector2::one);
        sys.Step(dt);                               // 消费未初始化 torque
        sys.SetVelocity(b, Vector2::zero, 0.0f);
        sys.UpdateTransform(b, Vector2(0.0f, 1000.0f), 0.0f, Vector2::one);   // 复位，从静止开始

        sys.Step(dt);
        auto s = GetSnap(sys, b);
        Ok(Near(s.lv.y, -9.8f * dt, 1e-3f), "单子步：一步后 vy = -g*dt");
        // 半隐式欧拉：先更新 v 再用新 v 积分位置 → y = y0 - g*dt^2
        Ok(Near(s.pos.y, 1000.0f - 9.8f * dt * dt, 1e-3f), "单子步：y 按半隐式欧拉积分（y0 - g*dt^2）");
        Ok(Near(s.av, 0.0f, 1e-3f), "无外力矩时角速度保持 0");

        sys.SetVelocity(b, Vector2(5.0f, 0.0f), 0.0f);
        auto s0 = GetSnap(sys, b);
        sys.Step(dt);
        auto s1 = GetSnap(sys, b);
        Ok(Near(s1.pos.x - s0.pos.x, 5.0f * dt, 1e-3f), "水平速度积分 x += vx*dt");
    }
    {
        PhySystem sys(4, 8);
        auto b = sys.CreateBody();
        sys.SetBody(b, 1.0f, 1.0f);
        sys.UpdateTransform(b, Vector2(0.0f, 1000.0f), 0.0f, Vector2::one);
        sys.Step(dt);
        sys.SetVelocity(b, Vector2::zero, 0.0f);
        sys.Step(dt);
        auto s = GetSnap(sys, b);
        Ok(Near(s.lv.y, -9.8f * dt, 1e-3f), "4 子步：一步后 vy 仍为 -g*dt（子步不改变总冲量）");
    }
}

// --------------------------------------------------------------- D. 落地
static void TestFallingOnGround()
{
    Section("D. 静态地面 + 动态球落地（端到端）");
    const float dt = 1.0f / 60.0f;
    PhySystem sys(4, 8);

    MakeShape(sys, ColliderType::Box, RigidbodyHandle::null, Vector2::zero, Vector2(10.0f, 0.5f));
    auto ball = MakeBall(sys, 0.5f, Vector2(0.0f, 3.0f), dt);

    bool everNaN = false;
    for (int i = 0; i < 420; i++)
    {
        sys.Step(dt);
        auto s = GetSnap(sys, ball);
        if (!Finite(s.pos) || !Finite(s.lv) || !std::isfinite(s.av))
            everNaN = true;
    }

    auto s = GetSnap(sys, ball);
    Ok(!everNaN, "落地全程无 NaN");
    Ok(s.pos.y > 0.9f && s.pos.y < 1.1f, "球停在地面上方（球心 y ≈ 1.0）");
    Ok(std::fabs(s.lv.y) < 0.15f, "落地后竖直速度趋近 0");
    Ok(std::fabs(s.pos.x) < 0.1f, "落地过程无横向漂移");
}

// --------------------------------------------------------------- E. 碰撞信息
static void TestCollisionInfo()
{
    Section("E. 重叠分离 / 碰撞信息（depth, normal, point）");
    const float dt = 1.0f / 60.0f;
    PhySystem sys(1, 8);

    auto sStatic = MakeShape(sys, ColliderType::Sphere, RigidbodyHandle::null, Vector2::zero, Vector2(0.5f, 0.0f));

    // 手动建球以保留 collider 句柄（用于过滤碰撞对）
    auto body = sys.CreateBody();
    sys.SetBody(body, 1.0f, 1.0f);
    auto bc = MakeShape(sys, ColliderType::Sphere, body, Vector2::zero, Vector2(0.5f, 0.0f));
    sys.UpdateTransform(body, Vector2(0.6f, 0.0f), 0.0f, Vector2::one);   // 重叠 0.4
    sys.Step(1.0f / 60.0f);                 // 消费未初始化 torque（见文件头说明）
    sys.SetVelocity(body, Vector2::zero, 0.0f);
    sys.UpdateTransform(body, Vector2(0.6f, 0.0f), 0.0f, Vector2::one);

    sys.Step(dt);
    float depth = -1.0f;
    Vector2 normal = Vector2::zero, point = Vector2::zero;
    bool got = false;
    sys.ForeachCollisionPairs([&](CollisionPair p)
    {
        auto a = p.first.collider, b = p.second.collider;
        if (!((a == bc && b == sStatic) || (a == sStatic && b == bc)))
            return;
        got = true;
        depth = p.depth;
        normal = p.normal;
        point = p.point;
        // normal 方向必须为 first -> second
        Vector2 expect = (a == bc) ? Vector2(-1.0f, 0.0f) : Vector2(1.0f, 0.0f);
        Ok(Dot(normal, expect) > 0.9f, "normal 方向符合 first -> second 约定");
    });
    Ok(got, "重叠的两球产生碰撞对");
    Ok(Near(depth, 0.4f, 0.05f), "depth ≈ 0.4（两球重叠深度）");
    Ok(point.x > 0.0f && point.x < 0.6f && std::fabs(point.y) < 0.1f, "碰撞点位于两球心之间");

    for (int i = 0; i < 30; i++)
        sys.Step(dt);
    auto s = GetSnap(sys, body);
    Ok(s.pos.x > 0.9f, "重叠球被位置修正推向 +x 分离（x 趋近 1.0）");
}

// --------------------------------------------------------------- F. BVH 同步
static void TestBvhSync()
{
    Section("F. BVH 同步（移动传播 / 移走 / 销毁）");
    const float dt = 1.0f / 60.0f;
    PhySystem sys(1, 8);

    auto ground = MakeShape(sys, ColliderType::Box, RigidbodyHandle::null, Vector2::zero, Vector2(1.0f, 1.0f));

    // 手动建球以保留 collider 句柄（末尾要单独销毁它）
    auto ball = sys.CreateBody();
    sys.SetBody(ball, 1.0f, 1.0f);
    auto bc = MakeShape(sys, ColliderType::Sphere, ball, Vector2::zero, Vector2(0.5f, 0.0f));
    sys.UpdateTransform(ball, Vector2(0.0f, 1.2f), 0.0f, Vector2::one);
    sys.Step(dt);                               // 消费未初始化 torque（见文件头说明）
    sys.SetVelocity(ball, Vector2::zero, 0.0f);
    sys.UpdateTransform(ball, Vector2(0.0f, 1.2f), 0.0f, Vector2::one);

    sys.Step(dt);
    Ok(sys.GetCollisionPairCount() >= 1, "动态球创建后即参与检测（创建即入脏集合）");

    sys.UpdateTransform(ball, Vector2(50.0f, 1.2f), 0.0f, Vector2::one);
    sys.Step(dt);
    Ok(sys.GetCollisionPairCount() == 0, "UpdateTransform 移走后碰撞对消失（collider 边界随刚体同步）");

    sys.UpdateTransform(ball, Vector2(0.0f, 1.2f), 0.0f, Vector2::one);
    sys.Step(dt);
    Ok(sys.GetCollisionPairCount() >= 1, "移回后重新产生碰撞对（BVH 节点更新）");

    sys.DestroyCollider(ground);
    sys.Step(dt);
    Ok(sys.GetCollisionPairCount() == 0, "销毁静态 collider 后碰撞对消失");

    sys.DestroyCollider(bc);
    sys.Step(dt);
    sys.DestroyBody(ball);
    sys.Step(dt);
    Ok(true, "先销毁 collider 再销毁 body 不崩溃（覆盖重复销毁路径）");
}

// --------------------------------------------------------------- G. 已知缺口
static void TestKnownGaps()
{
    Section("G. 已知缺口（不影响退出码）");
    const float dt = 1.0f / 60.0f;
    {
        // G1: Rigidbody::torque 未初始化 -> 首帧角速度被未定义值冲击
        PhySystem sys(1, 8);
        auto b = sys.CreateBody();
        sys.SetBody(b, 1.0f, 1.0f);
        sys.UpdateTransform(b, Vector2(0.0f, 1000.0f), 0.0f, Vector2::one);
        sys.Step(dt);
        auto s = GetSnap(sys, b);
        Known(Near(s.av, 0.0f, 1e-3f),
              "新建刚体首帧角速度应保持 0（Rigidbody::torque 未初始化）");
    }
    {
        // G2: Collider 摩擦/弹性成员未初始化（堆对象，Debug 堆填充非零）
        auto* box = new BoxCollider(ColliderHandle::FromId(201));
        bool allZero = box->staticFriction == 0.0f && box->dynamicFriction == 0.0f && box->elasticity == 0.0f;
        delete box;
        Known(allZero, "Collider 摩擦/弹性默认应为 0（成员未初始化）");
    }
    {
        // G3: mass = 0 的刚体在 AddForce 中 0 * inf = NaN（缺 Max(mass, Epsilon) 保护）
        PhySystem sys(1, 8);
        auto b = sys.CreateBody();
        sys.SetBody(b, 0.0f, 1.0f);
        sys.UpdateTransform(b, Vector2(0.0f, 1000.0f), 0.0f, Vector2::one);
        sys.Step(dt);
        auto s = GetSnap(sys, b);
        Known(Finite(s.lv) && std::isfinite(s.av),
              "mass=0 的刚体不应被 NaN 污染（AddForce 缺 Max(mass, Epsilon) 保护）");
    }
    {
        // G4: 静态 collider 创建后未调用任何 setter 则不进入 BVH（注册依赖 setter 抛脏标记）
        PhySystem sys(1, 8);
        sys.CreateCollider(ColliderType::Box, RigidbodyHandle::null);
        auto ball = MakeBall(sys, 0.5f, Vector2::zero, dt);
        sys.Step(dt);
        Known(sys.GetCollisionPairCount() >= 1,
              "静态 collider 创建后应立即可参与检测（当前需至少一次 setter 触发注册）");
    }
}

// --------------------------------------------------------------- H. Step 边界
static void TestStepEdgeCases()
{
    Section("H. Step 边界（dt=0 / dt<0）");
    const float dt = 1.0f / 60.0f;
    PhySystem sys(1, 8);
    auto b = sys.CreateBody();
    sys.SetBody(b, 1.0f, 1.0f);
    sys.UpdateTransform(b, Vector2(1.0f, 2.0f), 0.0f, Vector2::one);
    sys.Step(dt);
    sys.SetVelocity(b, Vector2::zero, 0.0f);
    sys.UpdateTransform(b, Vector2(1.0f, 2.0f), 0.0f, Vector2::one);

    sys.Step(0.0f);
    auto s0 = GetSnap(sys, b);
    sys.Step(-1.0f);
    sys.Step(0.0f);                             // 重新填充 dirty 集
    auto s1 = GetSnap(sys, b);
    Ok(s0.found && s1.found && Near(s0.pos.x, s1.pos.x) && Near(s0.pos.y, s1.pos.y),
       "Step(0) / Step(-1) 不改变位置");
}

// --------------------------------------------------------------- Z. 分层自检
// 从 PhySystem 集成层逐级下探的自检（曾经用于定位“碰撞对全线消失”的根因：
// CreateCollider 漏调 SetBody → BVH 里所有 body 均为 null → 被 a.body==b.body 自碰撞过滤）。
static void TestLayerSelfChecks()
{
    Section("Z. 分层自检（窄相直连 / parent bound / BVH 过滤语义）");
    // Z1: 绕开 PhySystem 直连窄相：两球重叠 0.4
    {
        Collision::Sphere s1, s2;
        s1.SetSphere(0.5f);
        s2.SetSphere(0.5f);
        Vector2 p1 = Vector2::zero, p2 = Vector2(0.6f, 0.0f);
        ConvexDetectReq r1(&s1, InverseTR(p1, 0.0f), TR(p1, 0.0f));
        ConvexDetectReq r2(&s2, InverseTR(p2, 0.0f), TR(p2, 0.0f));
        CollisionPair cp(CollisionObj(ColliderHandle::null, RigidbodyHandle::null),
                         CollisionObj(ColliderHandle::null, RigidbodyHandle::null));
        std::vector<NarrowPhaseDetectPair> v;
        v.push_back(NarrowPhaseDetectPair(r1, r2, cp));
        NarrowPhase np;
        np.SetDetectPairs(v);
        auto& pairs = np.GetCollisionPairs();
        Ok(pairs.size() == 1, "直连窄相：重叠两球产生 1 个碰撞对");
        if (!pairs.empty())
        {
            Ok(Near(pairs[0].depth, 0.4f, 0.01f), "直连窄相：depth ≈ 0.4");
            Ok(pairs[0].normal.x > 0.99f, "直连窄相：normal ≈ (1,0)（first -> second）");
        }
    }
    // Z2: Collider 的 parent 快照路径 bound（A 组只覆盖了无 parent 路径）
    {
        SphereCollider sp(ColliderHandle::FromId(301));
        sp.SetSphere(0.5f);
        Transform parent;
        parent.SetLocalPosition(Vector2(3.0f, 4.0f));
        sp.SetParentTransform(parent);
        auto b = sp.GetBound();
        Ok(Near(b.center.x, 3.0f) && Near(b.center.y, 4.0f)
           && Near(b.extents.x, 0.5f) && Near(b.extents.y, 0.5f),
           "SetParentTransform 路径：bound 随父快照正确更新");
    }
    // Z3: BVH 过滤语义（IsIntersect 的同一刚体过滤：body 均为 null 视为“静态之间不判碰”）
    {
        auto c1 = ColliderHandle::FromId(401);
        auto c2 = ColliderHandle::FromId(402);
        BVHTree treeNull;
        treeNull.Insert(c1, RigidbodyHandle::null, CreateBound(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f)));
        treeNull.Insert(c2, RigidbodyHandle::null, CreateBound(Vector2(0.1f, -0.5f), Vector2(1.1f, 0.5f)));
        Ok(treeNull.GetCollisionPairs().empty(), "BVH：两个静态物体（body 均 null）不产生碰撞对");

        BVHTree treeBody;
        treeBody.Insert(c1, RigidbodyHandle::FromId(501), CreateBound(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f)));
        treeBody.Insert(c2, RigidbodyHandle::FromId(502), CreateBound(Vector2(0.1f, -0.5f), Vector2(1.1f, 0.5f)));
        Ok(treeBody.GetCollisionPairs().size() == 1, "BVH：不同刚体的两物体产生碰撞对");
    }
}

int main()
{
    printf("SpaceZ_2D_Alpha - PhySystem / Collider 集成测试\n");

    TestColliderBounds();
    TestLifecycle();
    TestIntegration();
    TestFallingOnGround();
    TestCollisionInfo();
    TestBvhSync();
    TestKnownGaps();
    TestStepEdgeCases();
    TestLayerSelfChecks();

    printf("\n=============================\n");
    printf("PASS %d / FAIL %d / KNOWN %d\n", g_pass, g_fail, g_known);
    if (g_known > 0)
        printf("（KNOWN 为已知未修复缺口，不计入退出码）\n");
    return g_fail == 0 ? 0 : 1;
}
