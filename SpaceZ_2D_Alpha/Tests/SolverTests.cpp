// SpaceZ_2D_Alpha/Tests/SolverTests.cpp
// 物理求解器回归测试：只依赖 Core + 控制台，不需要 Unity。
// 构建：tools/build_tests.bat   ->  Tests/Debug/SpaceZ_SolverTests.exe
//
// 覆盖五层：
//   A. SolverPair 冲量算式（单元级，绕开容器）
//   B. PhySolver 集成（SolveStep + Foreach）
//   C. IslandDivider 并查集分岛
//   D. 边界输入与已知缺口
//   E. 位置修正 ApplyPositionCorrection（本轮改动的回归位）
// 标记约定：[PASS] 通过 / [FAIL] 回归（影响退出码）/ [KNOWN] 已知未修复（不影响退出码）
//
// !! 构造被测对象时的强制约定 !!
//   SolverRigidbody 只有 `SolverRigidbody()`（仅置 invMass/invInertia=0）和
//   `SolverRigidbody(m, i)`，**angle / position / linearVelocity / angularVelocity 都没有
//   默认初始化**。所以测试里一律走 MakeBody / MakeStatic / Contact 三个助手，
//   绝不裸构造后只赋一部分字段 —— 否则读到的是栈上的垃圾值，测试会随机通过/失败。
//
// !! 本轮 API 变化 !!
//   SolverPair 构造函数由 (point, normal) 变为
//   (point, normal, std::pair<Vector2,Vector2>{anchorA, anchorB})，
//   CollisionPair 新增 SetCollisionPoint(anchorA, anchorB)。

#include "Solver/PhySolver.h"
#include "Solver/IslandDivider.h"
#include "Solver/SolverPair.h"
#include "Solver/SolverRigidbody.h"
#include "Object/Rigidbody.h"
#include "Object/Collider.h"
#include "Collision/CollisionPair.h"
#include "Math/Vector2.h"
#include "Math/Math.h"

#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

using namespace Core;
using namespace Core::Math;
using namespace Core::Solver;
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
static bool Near(float a, float b, float eps = 1e-4f) { return std::fabs(a - b) <= eps; }

// --------------------------------------------------------------- 构造助手
static SolverRigidbody MakeBody(float m, float inertia, Vector2 pos, Vector2 v, float w)
{
    SolverRigidbody b(m, inertia);
    b.position = pos; b.linearVelocity = v; b.angle = 0.0f; b.angularVelocity = w;
    return b;
}
static SolverRigidbody MakeStatic(Vector2 pos)
{
    SolverRigidbody b;
    b.position = pos; b.linearVelocity = Vector2::zero; b.angle = 0.0f; b.angularVelocity = 0.0f;
    return b;
}
// 以 (point, normal) 构造碰撞对（anchor 默认取零向量）
static SolverPair MakeSolverPair(Vector2 point, Vector2 normal,
                                 Vector2 anchorA = Vector2::zero, Vector2 anchorB = Vector2::zero)
{
    return SolverPair(point, normal, {anchorA, anchorB});
}
// 设置接触材质（等价于走 PhySolver 时的 SetContact 路径）
static void Contact(SolverPair& p, float sf, float df, float e)
{
    Collider ca, cb;
    ca.staticFriction = sf; ca.dynamicFriction = df; ca.elasticity = e;
    ca.angle = 0.0f; ca.position = Vector2::zero;
    cb = ca;
    p.SetContact(ca, cb);
}
// point / normal 定好之后再算有效质量（SetRigidbody 内部按当前 point/normal 计算）
static void MakePair(SolverPair& p, SolverRigidbody& a, SolverRigidbody& b, Vector2 normal, Vector2 point)
{
    p.point = point;
    p.normal = normal.Normalized();
    p.SetRigidbody(&a, &b);
}

static void InitCol(Collider& c, float sf, float df, float e, Vector2 pos)
{
    c.staticFriction = sf; c.dynamicFriction = df; c.elasticity = e;
    c.angle = 0; c.position = pos;
}
static void InitBody(Rigidbody& b, float m, float inertia, Vector2 pos, Vector2 v, float w)
{
    b.mass = m; b.inertia = inertia; b.position = pos; b.linearVelocity = v;
    b.angle = 0; b.angularVelocity = w;
}

// ---------------------------------------------------------------- A1 法向冲量
static void TestNormalImpulse()
{
    Section("A1 法向冲量（单元级）");

    {   // 等质量正碰、完全非弹性：共同速度 = 0.5
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        SolverRigidbody b = MakeBody(1, 1, Vector2(1, 0), Vector2::zero, 0);
        SolverPair p = MakeSolverPair(Vector2(0.5f, 0), Vector2(1, 0));
        MakePair(p, a, b, Vector2(1, 0), Vector2(0.5f, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        Ok(Near(a.linearVelocity.x, 0.5f) && Near(b.linearVelocity.x, 0.5f),
           "非弹性等质量对撞 -> 双侧 0.5");
        Ok(Near(p.normalImpulse, 0.5f), "normalImpulse == 0.5");
    }
    {   // 弹性 e=1：等质量下速度互换
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        SolverRigidbody b = MakeBody(1, 1, Vector2(1, 0), Vector2::zero, 0);
        SolverPair p = MakeSolverPair(Vector2(0.5f, 0), Vector2(1, 0));
        MakePair(p, a, b, Vector2(1, 0), Vector2(0.5f, 0));
        Contact(p, 0.5f, 0.5f, 1.0f);
        p.ApplyNormalImpulse();
        Ok(Near(a.linearVelocity.x, 0.0f) && Near(b.linearVelocity.x, 1.0f),
           "弹性 e=1 等质量对撞 -> 速度互换 (0, 1)");
    }
    {   // 撞静态体
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        SolverRigidbody s = MakeStatic(Vector2(1, 0));
        SolverPair p = MakeSolverPair(Vector2(0.5f, 0), Vector2(1, 0));
        MakePair(p, a, s, Vector2(1, 0), Vector2(0.5f, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        Ok(Near(a.linearVelocity.x, 0.0f), "撞静态体 -> 动体速度归 0");
        Ok(Near(s.linearVelocity.x, 0.0f), "静态体不受冲量影响");
    }
    {   // 已在分离：不应产生冲量
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(-1, 0), 0);
        SolverRigidbody b = MakeBody(1, 1, Vector2(1, 0), Vector2::zero, 0);
        SolverPair p = MakeSolverPair(Vector2(0.5f, 0), Vector2(1, 0));
        MakePair(p, a, b, Vector2(1, 0), Vector2(0.5f, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        Ok(Near(p.normalImpulse, 0.0f) && Near(a.linearVelocity.x, -1.0f),
           "分离中的接触 -> 零冲量、速度不变");
    }
    {   // 偏心接触的解析解：crossA = -0.5, invEffM = 1.25, Jn = 0.8
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        SolverRigidbody s = MakeStatic(Vector2(0.5f, 0.5f));
        SolverPair p = MakeSolverPair(Vector2(0.5f, 0.5f), Vector2(1, 0));
        MakePair(p, a, s, Vector2(1, 0), Vector2(0.5f, 0.5f));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        Ok(Near(a.linearVelocity.x, 0.2f, 1e-3f), "偏心：线速度 x == 0.2（1 - Jn/m）");
        Ok(Near(a.angularVelocity, 0.4f, 1e-3f), "偏心：角速度 == 0.4（Cross(rA,-p)/I）");
    }
}

// ---------------------------------------------------------------- A2 摩擦冲量
// 统一场景：a 为动体(mass=1, inertia=1)、point 取在 a 质心，b 为静态体。
// 于是 invNormalEffMass = invTangentEffMass = 1；
// 法向 (0,1)，a.v = (vx, 0.5) => Jn = 0.5，摩擦上限 = mu * Jn。
static float FrictionResidualX(float vx, float mu)
{
    SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(vx, 0.5f), 0);
    SolverRigidbody s = MakeStatic(Vector2(0, 1));
    SolverPair p = MakeSolverPair(Vector2(0, 0), Vector2(0, 1));
    MakePair(p, a, s, Vector2(0, 1), Vector2(0, 0));
    Contact(p, mu, mu, 0.0f);
    p.ApplyNormalImpulse();                 // 先得到 normalImpulse = 0.5
    p.ApplyTangentImpulse(1.0f / 60.0f);
    return a.linearVelocity.x;
}

static void TestFrictionImpulse()
{
    Section("A2 摩擦冲量（单元级）—— 既修复的回归位");

    Ok(Near(FrictionResidualX(0.5f, 0.0f), 0.5f),
       "mu=0 -> 完全不减切向速度（0.5 保持 0.5）");
    Ok(Near(FrictionResidualX(1.0f, 0.0f), 1.0f),
       "mu=0 且快速滑动 -> 同样不减（1.0 保持 1.0）");

    Ok(Near(FrictionResidualX(1.0f, 0.5f), 0.75f),
       "|tP|=1.0 > 上限 0.25 -> 只减 0.25，残速 0.75");
    Ok(Near(FrictionResidualX(0.4f, 0.5f), 0.15f),
       "|tP|=0.4 > 上限 0.25 -> 只减 0.25，残速 0.15");
    Ok(Near(FrictionResidualX(0.05f, 0.5f), 0.0f),
       "|tP|=0.05 < 上限 0.25 -> 被摩擦完全吃掉");
    Ok(Near(FrictionResidualX(-1.0f, 0.5f), -0.75f),
       "反方向滑动对称 -> 残速 -0.75");

    {   // 直接核对恒等式：施加量 == min(|tP|, mu*Jn)
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1.0f, 0.5f), 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 1));
        SolverPair p = MakeSolverPair(Vector2(0, 0), Vector2(0, 1));
        MakePair(p, a, s, Vector2(0, 1), Vector2(0, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        float before = a.linearVelocity.x;
        p.ApplyTangentImpulse(1.0f / 60.0f);
        float applied = std::fabs(a.linearVelocity.x - before);
        float expected = Min(1.0f, 0.5f * p.normalImpulse);
        Ok(Near(applied, expected), "施加的摩擦冲量 == min(|tP|, mu*Jn)");
    }
    {   // 多轮迭代：法向冲量若被覆盖而非累计，第 2 轮摩擦预算会归零
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1.0f, 0.5f), 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 1));
        SolverPair p = MakeSolverPair(Vector2(0, 0), Vector2(0, 1));
        MakePair(p, a, s, Vector2(0, 1), Vector2(0, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        float j1 = p.normalImpulse;
        p.ApplyNormalImpulse();                 // 第 2 轮：nV 已打平
        Ok(j1 > 0.4f && Near(p.normalImpulse, j1),
              "法向冲量跨轮累积：第 2 轮 nV 打平后 Jn 仍保持（摩擦预算不归零）");
    }
}

// ---------------------------------------------------------------- A3 多轮迭代
// 引擎默认 _iteration=4，而 A2 的每个用例只跑了「单轮」。本节点补上多轮：摩擦
// 削减量必须与迭代轮数无关（既不过制动、也不被回推抵消）。
static float FrictionResidualXN(float vx, float mu, int N)
{
    SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(vx, 0.5f), 0);
    SolverRigidbody s = MakeStatic(Vector2(0, 1));
    SolverPair p = MakeSolverPair(Vector2(0, 0), Vector2(0, 1));
    MakePair(p, a, s, Vector2(0, 1), Vector2(0, 0));
    Contact(p, mu, mu, 0.0f);
    for (int i = 0; i < N; ++i)
    {
        p.ApplyNormalImpulse();
        p.ApplyTangentImpulse(1.0f / 60.0f);
    }
    return a.linearVelocity.x;
}

static void TestFrictionIterationInvariance()
{
    Section("A3 摩擦的迭代次数无关性（多轮迭代）");

    // 远超摩擦上限（vx=3.0 > mu*Jn=0.25）：削减量应恒为 0.25，轮数再多也不过制动
    Ok(Near(FrictionResidualXN(3.0f, 0.5f, 1), 2.75f),  "vx=3.0 N=1  残速 2.75（削减 0.25）");
    Ok(Near(FrictionResidualXN(3.0f, 0.5f, 4), 2.75f),  "vx=3.0 N=4  残速 2.75（与 N=1 一致）");
    Ok(Near(FrictionResidualXN(3.0f, 0.5f, 16), 2.75f), "vx=3.0 N=16 残速 2.75（与 N=1 一致）");

    {   // 法向冲量跨轮累积：既不归零、也不膨胀
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1.0f, 0.5f), 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 1));
        SolverPair p = MakeSolverPair(Vector2(0, 0), Vector2(0, 1));
        MakePair(p, a, s, Vector2(0, 1), Vector2(0, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        for (int i = 0; i < 50; ++i) p.ApplyNormalImpulse();
        Ok(Near(p.normalImpulse, 0.5f), "50 轮后 Jn 稳定在 0.5（不归零、不膨胀）");
    }

    // 残速落入摩擦锥内（vx=0.4 -> 首轮后 0.15 < 0.25）：累积量必须先累加再夹锥，
    // 否则 N 为偶数时会把已施加的切向冲量回推（削 0.15 而非 0.25）。
    Ok(Near(FrictionResidualXN(0.4f, 0.5f, 1), FrictionResidualXN(0.4f, 0.5f, 4)) &&
       Near(FrictionResidualXN(0.4f, 0.5f, 1), FrictionResidualXN(0.4f, 0.5f, 16)),
       "vx=0.4：削减量与迭代轮数无关（N=1/4/16 残速恒为 0.15）");
}

static void TestSolverIntegration()
{
    Section("B1 PhySolver 集成（SolveStep + Foreach）");

    {   // 两个动态体正碰
        Rigidbody a(RigidbodyHandle::FromId(0)), b(RigidbodyHandle::FromId(1));
        InitBody(a, 1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        InitBody(b, 1, 1, Vector2(1, 0), Vector2::zero, 0);
        Collider ca, cb;
        InitCol(ca, 0.5f, 0.5f, 0.0f, a.position); ca.SetBody(a.Handle());
        InitCol(cb, 0.5f, 0.5f, 0.0f, b.position); cb.SetBody(b.Handle());
        CollisionPair cp;
        cp.SetCollisionRigidbody(&a, &b); cp.SetCollisionCollider(&ca, &cb);
        cp.SetCollisionInfo(Vector2(0.5f, 0), Vector2(1, 0), 0.5f);
        std::vector<CollisionPair> pairs{cp};

        PhySolver solver(4);
        solver.SolveStep(pairs, 1.0f / 60.0f);

        int n = 0; float vsum = 0.0f;
        solver.Foreach([&](const RigidbodyHandle&, const SolverRigidbody* const s) {
            ++n; vsum += s->linearVelocity.x;
        });
        Ok(n == 2, "两个动态体 -> Foreach 枚举 2 个求解体");
        Ok(Near(vsum, 1.0f), "动量守恒：两侧速度之和 == 1.0");
    }
    {   // 动态 vs 静态
        Rigidbody a(RigidbodyHandle::FromId(0));
        InitBody(a, 1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        Collider ca, cs;
        InitCol(ca, 0.5f, 0.5f, 0.0f, a.position); ca.SetBody(a.Handle());
        InitCol(cs, 0.5f, 0.5f, 0.0f, Vector2(1, 0));       // 无 body = 静态
        CollisionPair cp;
        cp.SetCollisionRigidbody(&a, nullptr); cp.SetCollisionCollider(&ca, &cs);
        cp.SetCollisionInfo(Vector2(0.5f, 0), Vector2(1, 0), 0.5f);
        std::vector<CollisionPair> pairs{cp};

        PhySolver solver(4);
        solver.SolveStep(pairs, 1.0f / 60.0f);
        int n = 0; bool stopped = false;
        solver.Foreach([&](const RigidbodyHandle&, const SolverRigidbody* const s) {
            ++n; stopped = Near(s->linearVelocity.x, 0.0f);
        });
        Ok(n == 1, "静态碰撞体不进 Foreach（只枚举动态体）");
        Ok(stopped, "撞静态体 -> 动体速度归 0");
    }
    {   // 空输入
        PhySolver solver(4);
        std::vector<CollisionPair> empty;
        solver.SolveStep(empty, 1.0f / 60.0f);
        int n = 0;
        solver.Foreach([&](const RigidbodyHandle&, const SolverRigidbody* const) { ++n; });
        Ok(n == 0, "空碰撞对 -> 不崩且无求解体");
    }
}

static void TestSolverContinuity()
{
    Section("B2 多帧复用 / 对象池");

    Rigidbody a(RigidbodyHandle::FromId(0)), b(RigidbodyHandle::FromId(1));
    Collider ca, cb;
    PhySolver solver(4);
    bool allOk = true;
    for (int f = 0; f < 120; ++f)
    {
        InitBody(a, 1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        InitBody(b, 1, 1, Vector2(1, 0), Vector2::zero, 0);
        InitCol(ca, 0.5f, 0.5f, 0.0f, a.position); ca.SetBody(a.Handle());
        InitCol(cb, 0.5f, 0.5f, 0.0f, b.position); cb.SetBody(b.Handle());

        CollisionPair cp;
        cp.SetCollisionRigidbody(&a, &b); cp.SetCollisionCollider(&ca, &cb);
        cp.SetCollisionInfo(Vector2(0.5f, 0), Vector2(1, 0), 0.5f);
        std::vector<CollisionPair> pairs{cp};
        if (f % 3 == 0) pairs.push_back(cp);        // 偶尔多一个接触点

        solver.SolveStep(pairs, 1.0f / 60.0f);
        int n = 0; float vsum = 0.0f;
        solver.Foreach([&](const RigidbodyHandle&, const SolverRigidbody* const s) {
            ++n; vsum += s->linearVelocity.x;
        });
        if (n != 2 || !Near(vsum, 1.0f)) allOk = false;
    }
    Ok(allOk, "120 帧（每帧 1~2 个接触）结果稳定，无悬挂指针/状态残留");
}

// ---------------------------------------------------------------- C 分岛
static void TestIslandDivider()
{
    Section("C1 IslandDivider 并查集");

    SolverRigidbody sa = MakeBody(1, 1, Vector2(0, 0), Vector2::zero, 0);
    SolverRigidbody sb = MakeBody(1, 1, Vector2(1, 0), Vector2::zero, 0);
    SolverRigidbody sc = MakeBody(1, 1, Vector2(2, 0), Vector2::zero, 0);
    SolverRigidbody sd = MakeBody(1, 1, Vector2(3, 0), Vector2::zero, 0);

    SolverPair pab = MakeSolverPair(Vector2(0.5f, 0),  Vector2(1, 0)); pab.SetRigidbody(&sa, &sb);
    SolverPair pbc = MakeSolverPair(Vector2(1.5f, 0),  Vector2(1, 0)); pbc.SetRigidbody(&sb, &sc);
    SolverPair pcd = MakeSolverPair(Vector2(10.5f, 0), Vector2(1, 0)); pcd.SetRigidbody(&sc, &sd);

    IslandDivider divider;
    Ok(divider.IslandDivision({pab, pcd}).size() == 2,      "两组互不相连 -> 2 个岛");
    Ok(divider.IslandDivision({pab, pbc}).size() == 1,      "链式 A-B, B-C -> 1 个岛");
    Ok(divider.IslandDivision({pab, pbc, pcd}).size() == 1, "链式 A-B, B-C, C-D -> 1 个岛");
    Ok(divider.IslandDivision({pab}).size() == 1,           "单个碰撞对 -> 1 个岛");
    Ok(divider.IslandDivision({}).size() == 0,              "空输入 -> 0 个岛");
}

// ---------------------------------------------------------------- E 位置修正
// 本轮改动新增 SolverPair::ApplyPositionCorrection()：
//   worldA = first.PointLocalToWorld(firstAnchorPoint)      （本地锚点 -> 世界）
//   worldB = second.PointLocalToWorld(secondAnchorPoint)
//   depth  = Dot(worldA - worldB, normal)                   （>0 表示重叠）
//   d      = Clamp(depth - PENETRATE_SLOP, 0, MAX_POS_CORRECTION)
//   p      = RELAXATION * d / invNormalEffMass * normal
//   first.position  -= invMass * p ; second.position += invMass * p
//   angle += invInertia * Cross(r, ∓p)
// 常量：RELAXATION=0.2  MAX_POS_CORRECTION=0.2  PENETRATE_SLOP=0.005
//
// 统一场景：a 为动体(mass=1,inertia=1) 置于原点、s 为静态体，normal=(1,0)，
// 接触点取在原点 => crossA = 0 => invNormalEffMass = 1，于是 p = 0.2 * d。
static SolverPair MakePosPair(SolverRigidbody& a, SolverRigidbody& b,
                              Vector2 normal, Vector2 contactPoint,
                              Vector2 anchorA, Vector2 anchorB)
{
    SolverPair p(contactPoint, normal, {anchorA, anchorB});
    p.point = contactPoint;
    p.normal = normal.Normalized();
    p.SetRigidbody(&a, &b);
    return p;
}

static void TestPositionCorrection()
{
    Section("E1 位置修正（单元级）—— 本轮改动的回归位");

    {   // 解析解：depth = 0.105 -> d = 0.1 -> p = 0.02 -> 动体 x = -0.02
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2::zero, 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 0));
        SolverPair p = MakePosPair(a, s, Vector2(1, 0), Vector2(0, 0),
                                   Vector2(0.605f, 0), Vector2(0.5f, 0));
        Ok(Near(p.invNormalEffMass, 1.0f), "有效质量倒数 == 1（动体 m=1，静态体 0）");
        p.ApplyPositionCorrection();
        Ok(Near(a.position.x, -0.02f, 1e-5f), "depth=0.105 -> 动体被推开 0.02（解析解）");
        Ok(Near(a.position.y, 0.0f), "沿法线方向 -> 切向不动");
        Ok(Near(a.angle, 0.0f), "锚点在 x 轴上 -> 无力矩");
        Ok(Near(s.position.x, 0.0f), "静态体 invMass=0 -> 位置不变");
    }
    {   // 深度小于允许穿透：不产生任何修正
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2::zero, 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 0));
        SolverPair p = MakePosPair(a, s, Vector2(1, 0), Vector2(0, 0),
                                   Vector2(0.503f, 0), Vector2(0.5f, 0));
        p.ApplyPositionCorrection();
        Ok(Near(a.position.x, 0.0f), "depth=0.003 < slop 0.005 -> 完全不动");
    }
    {   // 深度超过单轮上限：被 MAX_POS_CORRECTION 截断
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2::zero, 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 0));
        SolverPair p = MakePosPair(a, s, Vector2(1, 0), Vector2(0, 0),
                                   Vector2(1.505f, 0), Vector2(0.5f, 0));
        p.ApplyPositionCorrection();
        Ok(Near(a.position.x, -0.04f, 1e-5f),
           "depth=1.005 -> 单轮修正被截到 MAX_POS_CORRECTION=0.2 -> 位移 0.04");
    }
    {   // 分离中的物体（depth < 0）：不得被拉近
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2::zero, 0);
        SolverRigidbody s = MakeStatic(Vector2(1, 0));
        SolverPair p = MakePosPair(a, s, Vector2(1, 0), Vector2(0.5f, 0),
                                   Vector2::zero, Vector2::zero);
        p.ApplyPositionCorrection();
        Ok(Near(a.position.x, 0.0f), "已分离（depth = -1）-> 不被误修正");
    }
    {   // 角度修正：锚点偏离质心连线 -> Cross(rA, -p) 产生力矩
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2::zero, 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 0));
        SolverPair p = MakePosPair(a, s, Vector2(1, 0), Vector2(0, 0),
                                   Vector2(0.605f, 0.5f), Vector2(0.5f, 0));
        p.ApplyPositionCorrection();
        // rA = (0.605, 0.5)，p = (0.02, 0) -> angle += invInertia * (0.5 * 0.02) = 0.01
        Ok(Near(a.angle, 0.01f, 1e-5f), "锚点偏离连线 -> 角度修正 0.01（Cross(rA,-p)/I）");
        Ok(Near(a.position.x, -0.02f, 1e-5f), "角度修正不影响平移量");
    }
    {   // 多轮弛豫：每轮吃掉剩余深度的 20%，收敛到 slop 且不越过
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2::zero, 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 0));
        SolverPair p = MakePosPair(a, s, Vector2(1, 0), Vector2(0, 0),
                                   Vector2(0.605f, 0), Vector2(0.5f, 0));
        bool monotonic = true;
        float prev = a.position.x;
        for (int i = 0; i < 60; ++i)
        {
            p.ApplyPositionCorrection();
            if (a.position.x > prev + 1e-6f) monotonic = false;
            prev = a.position.x;
        }
        float depth = (a.position.x + 0.605f) - 0.5f;
        Ok(monotonic, "60 轮弛豫单调推进（不来回振荡）");
        Ok(Near(depth, 0.005f, 1e-3f), "收敛到允许穿透 PENETRATE_SLOP=0.005");
    }
    {   // 反复施加不产生非有限值
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2::zero, 0);
        SolverRigidbody s = MakeStatic(Vector2(0, 0));
        SolverPair p = MakePosPair(a, s, Vector2(1, 0), Vector2(0, 0),
                                   Vector2(100.0f, 0), Vector2(0.5f, 0));
        for (int i = 0; i < 200; ++i) p.ApplyPositionCorrection();
        Ok(std::isfinite(a.position.x) && std::isfinite(a.angle),
           "极大穿透 200 轮 -> 位置/角度保持有限值");
    }
}

// ---------------------------------------------------------------- E2 集成
static void TestPositionCorrectionIntegration()
{
    Section("E2 位置修正（PhySolver 集成）");

    Rigidbody a(RigidbodyHandle::FromId(0));
    InitBody(a, 1, 1, Vector2(0, 0), Vector2::zero, 0);
    Collider ca, cs;
    InitCol(ca, 0.5f, 0.5f, 0.0f, a.position); ca.SetBody(a.Handle());
    InitCol(cs, 0.5f, 0.5f, 0.0f, Vector2(0, 0));       // 静态体

    CollisionPair cp;
    cp.SetCollisionRigidbody(&a, nullptr); cp.SetCollisionCollider(&ca, &cs);
    cp.SetCollisionInfo(Vector2(0, 0), Vector2(1, 0), 0.5f);
    cp.SetCollisionPoint(Vector2(0.605f, 0), Vector2(0.5f, 0));
    std::vector<CollisionPair> pairs{cp};

    PhySolver solver(4);
    solver.SolveStep(pairs, 1.0f / 60.0f);

    // depth0 = 0.105 -> e0 = 0.1，4 轮后 e = 0.1 * 0.8^4 = 0.04096
    // 位移 = -(0.1 - 0.04096) = -0.05904
    bool checked = false;
    solver.Foreach([&](const RigidbodyHandle&, const SolverRigidbody* const s) {
        checked = true;
        Ok(Near(s->position.x, -0.05904f, 1e-3f),
           "SolveStep 4 轮位置修正 -> 求解体 x = -0.05904（弛豫收敛解析解）");
    });
    Ok(checked, "Foreach 能取到求解体（位置修正未破坏对象池）");

    // 结果是否写回原 Rigidbody
    Known(Near(a.position.x, -0.05904f, 1e-3f), "SolveStep 把位置修正写回原 Rigidbody");
}

// ---------------------------------------------------------------- D 边界 / 已知缺口
static void TestEdgeCases()
{
    Section("D1 边界输入与已知缺口");

    // 一个动态体撞位于 (1,0) 的静态体
    auto buildPairs = [](Rigidbody& a, Collider& ca, Collider& cs,
                         Vector2 normal, std::vector<CollisionPair>& out) {
        InitBody(a, 1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        InitCol(ca, 0.5f, 0.5f, 0.0f, a.position); ca.SetBody(a.Handle());
        InitCol(cs, 0.5f, 0.5f, 0.0f, Vector2(1, 0));
        CollisionPair cp;
        cp.SetCollisionRigidbody(&a, nullptr); cp.SetCollisionCollider(&ca, &cs);
        cp.SetCollisionInfo(Vector2(0.5f, 0), normal, 0.5f);
        out.push_back(cp);
    };
    auto finiteAfter = [](std::vector<CollisionPair>& pairs, float dt) {
        PhySolver solver(4);
        solver.SolveStep(pairs, dt);
        bool finite = true;
        solver.Foreach([&](const RigidbodyHandle&, const SolverRigidbody* const s) {
            if (!std::isfinite(s->linearVelocity.x) || !std::isfinite(s->linearVelocity.y)
                || !std::isfinite(s->angularVelocity) || !std::isfinite(s->position.x)) finite = false;
        });
        return finite;
    };

    {   // 零长度法线
        Rigidbody a(RigidbodyHandle::FromId(0));
        Collider ca, cs;
        std::vector<CollisionPair> pairs;
        buildPairs(a, ca, cs, Vector2(0, 0), pairs);
        Ok(finiteAfter(pairs, 1.0f / 60.0f), "零长度法线 -> 速度/位置保持有限值");
    }
    {   // dt = 0
        Rigidbody a(RigidbodyHandle::FromId(0));
        Collider ca, cs;
        std::vector<CollisionPair> pairs;
        buildPairs(a, ca, cs, Vector2(1, 0), pairs);
        Ok(finiteAfter(pairs, 0.0f), "dt = 0 不产生 NaN（step 已下夹到 Epsilon，无 inf*0）");
    }
    {   // 结果是否写回原对象
        Rigidbody a(RigidbodyHandle::FromId(0));
        Collider ca, cs;
        std::vector<CollisionPair> pairs;
        buildPairs(a, ca, cs, Vector2(1, 0), pairs);
        PhySolver solver(4);
        solver.SolveStep(pairs, 1.0f / 60.0f);
        Known(Near(a.linearVelocity.x, 0.0f), "SolveStep 把速度结果写回原 Rigidbody");
    }
}

int main()
{
    printf("SpaceZ-2D 物理求解器测试\n");

    TestNormalImpulse();
    TestFrictionImpulse();
    TestFrictionIterationInvariance();
    TestSolverIntegration();
    TestSolverContinuity();
    TestIslandDivider();
    TestPositionCorrection();
    TestPositionCorrectionIntegration();
    TestEdgeCases();

    printf("\n----------------------------------------\n");
    printf("通过 %d   失败 %d   已知未修复 %d\n", g_pass, g_fail, g_known);
    printf("结果：%s\n", g_fail == 0 ? "OK" : "有回归");
    return g_fail == 0 ? 0 : 1;
}
