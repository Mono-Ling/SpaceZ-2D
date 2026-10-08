// SpaceZ_2D_Alpha/Tests/SolverTests.cpp
// 物理求解器回归测试：只依赖 Core + 控制台，不需要 Unity。
// 构建：tools/build_tests.bat   ->  Tests/Debug/SpaceZ_SolverTests.exe
//
// 覆盖三层：
//   A. SolverPair 冲量算式（单元级，绕开容器）
//   B. PhySolver 集成（SolveStep + Foreach）
//   C. IslandDivider 并查集分岛
//   D. 边界输入与已知缺口
// 标记约定：[PASS] 通过 / [FAIL] 回归（影响退出码）/ [KNOWN] 已知未修复（不影响退出码）
//
// !! 构造被测对象时的强制约定 !!
//   SolverRigidbody 只有 `SolverRigidbody()`（仅置 invMass/invInertia=0）和
//   `SolverRigidbody(m, i)`，**angle / position / linearVelocity / angularVelocity 都没有
//   默认初始化**；SolverPair 的 staticFriction / dynamicFriction / elasticity /
//   invNormalEffMass / invTangentEffMass 同样没有。
//   所以测试里一律走 MakeBody / MakeStatic / Contact 三个助手，绝不裸构造后只赋一部分字段
//   —— 否则读到的是栈上的垃圾值，测试会随栈内容随机通过/失败（实测踩过）。

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
        SolverPair p(Vector2(0.5f, 0), Vector2(1, 0));
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
        SolverPair p(Vector2(0.5f, 0), Vector2(1, 0));
        MakePair(p, a, b, Vector2(1, 0), Vector2(0.5f, 0));
        Contact(p, 0.5f, 0.5f, 1.0f);
        p.ApplyNormalImpulse();
        Ok(Near(a.linearVelocity.x, 0.0f) && Near(b.linearVelocity.x, 1.0f),
           "弹性 e=1 等质量对撞 -> 速度互换 (0, 1)");
    }
    {   // 撞静态体
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        SolverRigidbody s = MakeStatic(Vector2(1, 0));
        SolverPair p(Vector2(0.5f, 0), Vector2(1, 0));
        MakePair(p, a, s, Vector2(1, 0), Vector2(0.5f, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        Ok(Near(a.linearVelocity.x, 0.0f), "撞静态体 -> 动体速度归 0");
        Ok(Near(s.linearVelocity.x, 0.0f), "静态体不受冲量影响");
    }
    {   // 已在分离：不应产生冲量
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(-1, 0), 0);
        SolverRigidbody b = MakeBody(1, 1, Vector2(1, 0), Vector2::zero, 0);
        SolverPair p(Vector2(0.5f, 0), Vector2(1, 0));
        MakePair(p, a, b, Vector2(1, 0), Vector2(0.5f, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        Ok(Near(p.normalImpulse, 0.0f) && Near(a.linearVelocity.x, -1.0f),
           "分离中的接触 -> 零冲量、速度不变");
    }
    {   // 偏心接触的解析解：crossA = -0.5, invEffM = 1.25, Jn = 0.8
        SolverRigidbody a = MakeBody(1, 1, Vector2(0, 0), Vector2(1, 0), 0);
        SolverRigidbody s = MakeStatic(Vector2(0.5f, 0.5f));
        SolverPair p(Vector2(0.5f, 0.5f), Vector2(1, 0));
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
    SolverPair p(Vector2(0, 0), Vector2(0, 1));
    MakePair(p, a, s, Vector2(0, 1), Vector2(0, 0));
    Contact(p, mu, mu, 0.0f);
    p.ApplyNormalImpulse();                 // 先得到 normalImpulse = 0.5
    p.ApplyTangentImpulse(1.0f / 60.0f);
    return a.linearVelocity.x;
}

static void TestFrictionImpulse()
{
    Section("A2 摩擦冲量（单元级）—— 本次修复的回归位");

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
        SolverPair p(Vector2(0, 0), Vector2(0, 1));
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
        SolverPair p(Vector2(0, 0), Vector2(0, 1));
        MakePair(p, a, s, Vector2(0, 1), Vector2(0, 0));
        Contact(p, 0.5f, 0.5f, 0.0f);
        p.ApplyNormalImpulse();
        float j1 = p.normalImpulse;
        p.ApplyNormalImpulse();                 // 第 2 轮：nV 已打平
        Known(j1 > 0.4f && Near(p.normalImpulse, j1),
              "第 2 轮迭代仍保有累计法向冲量（摩擦预算不归零）");
    }
}

// ---------------------------------------------------------------- B1 集成
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

    SolverPair pab(Vector2(0.5f, 0),  Vector2(1, 0)); pab.SetRigidbody(&sa, &sb);
    SolverPair pbc(Vector2(1.5f, 0),  Vector2(1, 0)); pbc.SetRigidbody(&sb, &sc);
    SolverPair pcd(Vector2(10.5f, 0), Vector2(1, 0)); pcd.SetRigidbody(&sc, &sd);

    IslandDivider divider;
    Ok(divider.IslandDivision({pab, pcd}).size() == 2,      "两组互不相连 -> 2 个岛");
    Ok(divider.IslandDivision({pab, pbc}).size() == 1,      "链式 A-B, B-C -> 1 个岛");
    Ok(divider.IslandDivision({pab, pbc, pcd}).size() == 1, "链式 A-B, B-C, C-D -> 1 个岛");
    Ok(divider.IslandDivision({pab}).size() == 1,           "单个碰撞对 -> 1 个岛");
    Ok(divider.IslandDivision({}).size() == 0,              "空输入 -> 0 个岛");
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
                || !std::isfinite(s->angularVelocity)) finite = false;
        });
        return finite;
    };

    {   // 零长度法线
        Rigidbody a(RigidbodyHandle::FromId(0));
        Collider ca, cs;
        std::vector<CollisionPair> pairs;
        buildPairs(a, ca, cs, Vector2(0, 0), pairs);
        Ok(finiteAfter(pairs, 1.0f / 60.0f), "零长度法线 -> 速度保持有限值");
    }
    {   // dt = 0
        Rigidbody a(RigidbodyHandle::FromId(0));
        Collider ca, cs;
        std::vector<CollisionPair> pairs;
        buildPairs(a, ca, cs, Vector2(1, 0), pairs);
        Known(finiteAfter(pairs, 0.0f), "dt = 0 不产生 NaN（现为 nF = nP/step -> inf*0）");
    }
    {   // 结果是否写回原对象
        Rigidbody a(RigidbodyHandle::FromId(0));
        Collider ca, cs;
        std::vector<CollisionPair> pairs;
        buildPairs(a, ca, cs, Vector2(1, 0), pairs);
        PhySolver solver(4);
        solver.SolveStep(pairs, 1.0f / 60.0f);
        Known(Near(a.linearVelocity.x, 0.0f), "SolveStep 把结果写回原 Rigidbody");
    }
}

int main()
{
    printf("SpaceZ-2D 物理求解器测试\n");

    TestNormalImpulse();
    TestFrictionImpulse();
    TestSolverIntegration();
    TestSolverContinuity();
    TestIslandDivider();
    TestEdgeCases();

    printf("\n----------------------------------------\n");
    printf("通过 %d   失败 %d   已知未修复 %d\n", g_pass, g_fail, g_known);
    printf("结果：%s\n", g_fail == 0 ? "OK" : "有回归");
    return g_fail == 0 ? 0 : 1;
}
