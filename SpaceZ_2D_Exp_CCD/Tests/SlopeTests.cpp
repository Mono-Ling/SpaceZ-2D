// SpaceZ-2D CCD —— 斜面 / 基本动力学专项测试
// 构建：tools\build_tests.bat
// 运行：Tests\Debug\SpaceZ_CcdTests.exe
// 说明：当前 Tests 目录只有一个测试入口，后续新增测试请合并到本文件或另行拆分
//       并在 build_tests.bat 中处理多入口问题。
#include "System/PhySystem.h"
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>

using namespace Core;
using Math::Vector2;

namespace
{
    constexpr float PI = 3.14159265358979f;
    constexpr float DT = 1.0f / 60.0f;

    int g_pass = 0;
    int g_fail = 0;

    void Check(bool ok, const char* name, const std::string& detail = {})
    {
        if(ok)
        {
            ++g_pass;
            printf("[PASS] %s %s\n", name, detail.c_str());
        }
        else
        {
            ++g_fail;
            printf("[FAIL] %s %s\n", name, detail.c_str());
        }
    }

    std::string Fmt(const char* fmt, ...)
    {
        char buffer[256] = {};
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        return buffer;
    }

    struct State
    {
        Vector2 p;
        Vector2 v;
        float r = 0.0f;
        float av = 0.0f;
    };

    State Snapshot(PhySystem& sys, const RigidbodyHandle& want)
    {
        State out;
        sys.ForeachDirtyBodies([&](const RigidbodyHandle& h, const Vector2& p, float r,
                                   const Vector2& v, float av)
        {
            if(h == want)
            {
                out.p = p;
                out.r = r;
                out.v = v;
                out.av = av;
            }
        });
        return out;
    }

    ColliderHandle MakeStaticBox(PhySystem& sys, const Vector2& pos, float rot, const Vector2& ext)
    {
        auto c = sys.CreateCollider(RigidbodyHandle::null, ColliderType::Box);
        sys.SetBoxCollider(c, ext);
        sys.UpdateTransform(c, pos, rot, {1.0f, 1.0f});
        return c;
    }

    RigidbodyHandle MakeBox(PhySystem& sys, const Vector2& pos, float rot, float half,
                            bool gravity, ColliderHandle* outCollider = nullptr)
    {
        auto b = sys.CreateBody();
        sys.SetBody(b, 1.0f, 1.0f, gravity);
        auto c = sys.CreateCollider(b, ColliderType::Box);
        sys.SetBoxCollider(c, {half, half});
        sys.UpdateTransform(b, pos, rot, {1.0f, 1.0f});
        if(outCollider)
            *outCollider = c;
        return b;
    }

    Vector2 ToLocal(const Vector2& p, const Vector2& origin, float angle)
    {
        Vector2 d = p - origin;
        float c = std::cos(angle);
        float s = std::sin(angle);
        return {c * d.x + s * d.y, -s * d.x + c * d.y};
    }

    Vector2 ToWorld(const Vector2& local, const Vector2& origin, float angle)
    {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return origin + Vector2{c * local.x - s * local.y, s * local.x + c * local.y};
    }

    struct SlopeMeasure
    {
        float along = 0.0f;     // 沿坡坐标（局部 x）
        float distance = 0.0f;  // 到坡面（局部 y = halfThick）的距离
        float expect = 0.0f;    // 按相对角度计算的支撑距离
        float penetration = 0.0f;
        float vAlong = 0.0f;
    };

    SlopeMeasure Measure(const State& st, const Vector2& origin, float angle,
                         float halfThick, float boxHalf)
    {
        SlopeMeasure m;
        auto local = ToLocal(st.p, origin, angle);
        m.along = local.x;
        m.distance = local.y - halfThick;
        float relative = st.r - angle;
        m.expect = boxHalf * (std::abs(std::cos(relative)) + std::abs(std::sin(relative)));
        m.penetration = m.expect - m.distance;
        m.vAlong = ToLocal(st.v, Vector2::zero, angle).x;
        return m;
    }

    struct SlopeSetup
    {
        Vector2 origin{0.0f, -5.0f};
        float angle = 0.0f;
        float halfThick = 0.5f;
        float boxHalf = 0.4f;
        float halfLen = 20.0f;
    };

    // 在坡面上放置一个与坡面对齐的箱子
    RigidbodyHandle PlaceOnSlope(PhySystem& sys, const SlopeSetup& s, float along, float mu)
    {
        ColliderHandle bc;
        auto body = MakeBox(sys, {0.0f, 0.0f}, s.angle, s.boxHalf, true, &bc);
        sys.SetCollider(bc, mu, mu, 0.0f);
        auto pos = ToWorld({along, s.halfThick + s.boxHalf}, s.origin, s.angle);
        sys.UpdateTransform(body, pos, s.angle, {1.0f, 1.0f});
        return body;
    }

    // ============================ 基本动力学 ============================
    void TestKinematics()
    {
        {
            PhySystem sys(4, 8);
            auto b = MakeBox(sys, {0.0f, 0.0f}, 0.0f, 0.5f, false);
            sys.SetVelocity(b, {1.0f, 0.0f}, 0.0f);
            for(int i = 0; i < 60; ++i)
                sys.Step(DT);
            auto st = Snapshot(sys, b);
            Check(std::abs(st.p.x - 1.0f) < 0.02f, "kinematics/translate",
                  Fmt("x=%.3f expect 1.000", st.p.x));
        }
        {
            PhySystem sys(4, 8);
            auto b = MakeBox(sys, {0.0f, 0.0f}, 0.0f, 0.5f, false);
            sys.SetVelocity(b, {0.0f, 0.0f}, 1.0f);
            for(int i = 0; i < 60; ++i)
                sys.Step(DT);
            auto st = Snapshot(sys, b);
            Check(std::abs(st.r - 0.975f) < 0.03f, "kinematics/rotate",
                  Fmt("rot=%.3f expect ~0.975", st.r));
        }
        {
            PhySystem sys(4, 8);
            auto b = MakeBox(sys, {0.0f, 0.0f}, 0.0f, 0.5f, false);
            sys.SetVelocity(b, {1.0f, 0.0f}, 1.0f);
            for(int i = 0; i < 60; ++i)
                sys.Step(DT);
            auto st = Snapshot(sys, b);
            Check(std::abs(st.p.x - 1.0f) < 0.02f && std::abs(st.r - 0.975f) < 0.03f,
                  "kinematics/translate+rotate", Fmt("x=%.3f rot=%.3f", st.p.x, st.r));
        }
        {
            PhySystem sys(4, 8);
            auto b = MakeBox(sys, {0.0f, 0.0f}, 0.0f, 0.5f, true);
            for(int i = 0; i < 30; ++i)
                sys.Step(DT);
            auto st = Snapshot(sys, b);
            Check(st.p.y < -1.15f && st.p.y > -1.35f, "kinematics/gravity",
                  Fmt("y=%.4f expect ~-1.235", st.p.y));
        }
    }

    // ============================ 斜面临界 ============================
    void TestSlopeStatic(float degrees, float mu, float maxDrift, const char* name)
    {
        SlopeSetup s;
        s.angle = degrees * PI / 180.0f;
        PhySystem sys(4, 8);
        auto ramp = MakeStaticBox(sys, s.origin, s.angle, {s.halfLen, s.halfThick});
        sys.SetCollider(ramp, mu, mu, 0.0f);
        auto body = PlaceOnSlope(sys, s, 0.0f, mu);

        float maxPen = 0.0f;
        for(int f = 0; f < 180; ++f)
        {
            sys.Step(DT);
            auto m = Measure(Snapshot(sys, body), s.origin, s.angle, s.halfThick, s.boxHalf);
            maxPen = std::max(maxPen, m.penetration);
        }
        auto m = Measure(Snapshot(sys, body), s.origin, s.angle, s.halfThick, s.boxHalf);
        Check(std::abs(m.along) < maxDrift && maxPen < 0.03f, name,
              Fmt("drift=%.3f (limit %.2f) maxPen=%.3f", m.along, maxDrift, maxPen));
    }

    // ============================ 斜面滑落 ============================
    void TestSlopeSlide(float degrees, float mu, float expectedA, float tolerance, const char* name)
    {
        SlopeSetup s;
        s.angle = degrees * PI / 180.0f;
        s.halfLen = 60.0f;
        PhySystem sys(4, 8);
        auto ramp = MakeStaticBox(sys, s.origin, s.angle, {s.halfLen, s.halfThick});
        sys.SetCollider(ramp, mu, mu, 0.0f);
        auto body = PlaceOnSlope(sys, s, 20.0f, mu);   // 从坡上方开始

        float v1 = 0.0f;
        float v2 = 0.0f;
        float maxPen = 0.0f;
        for(int f = 0; f < 120; ++f)
        {
            sys.Step(DT);
            auto m = Measure(Snapshot(sys, body), s.origin, s.angle, s.halfThick, s.boxHalf);
            maxPen = std::max(maxPen, m.penetration);
            if(f == 59)
                v1 = m.vAlong;
            if(f == 119)
                v2 = m.vAlong;
        }
        float measured = std::abs(v2 - v1);   // 1 秒间隔
        auto m = Measure(Snapshot(sys, body), s.origin, s.angle, s.halfThick, s.boxHalf);
        bool ok = std::abs(measured - expectedA) < tolerance
               && maxPen < 0.03f
               && m.along > -s.halfLen + 1.0f;
        Check(ok, name, Fmt("a=%.3f (expect %.3f) maxPen=%.3f along=%.2f",
                            measured, expectedA, maxPen, m.along));
    }

    // ============================ 落到斜面 ============================
    void TestSlopeDrop()
    {
        SlopeSetup s;
        s.angle = 15.0f * PI / 180.0f;
        PhySystem sys(4, 8);
        auto ramp = MakeStaticBox(sys, s.origin, s.angle, {s.halfLen, s.halfThick});
        sys.SetCollider(ramp, 0.6f, 0.5f, 0.0f);

        ColliderHandle bc;
        auto body = MakeBox(sys, {0.0f, 0.0f}, 0.0f, s.boxHalf, true, &bc);
        sys.SetCollider(bc, 0.6f, 0.5f, 0.0f);
        sys.UpdateTransform(body, {0.0f, 0.5f}, 0.0f, {1.0f, 1.0f});

        float maxPen = 0.0f;
        float minD = 1e9f;
        for(int f = 0; f < 240; ++f)
        {
            sys.Step(DT);
            auto m = Measure(Snapshot(sys, body), s.origin, s.angle, s.halfThick, s.boxHalf);
            maxPen = std::max(maxPen, m.penetration);
            minD = std::min(minD, m.distance);
        }
        auto m = Measure(Snapshot(sys, body), s.origin, s.angle, s.halfThick, s.boxHalf);
        Check(maxPen < 0.05f && minD > 0.2f && std::abs(m.along) < s.halfLen - 1.0f,
              "slope/drop-15deg",
              Fmt("maxPen=%.3f minD=%.3f along=%.2f", maxPen, minD, m.along));
    }

    // ============================ 斜坡 + 挡墙 ============================
    void TestSlopeWall()
    {
        SlopeSetup s;
        s.angle = 20.0f * PI / 180.0f;
        PhySystem sys(4, 8);
        auto ramp = MakeStaticBox(sys, s.origin, s.angle, {s.halfLen, s.halfThick});
        sys.SetCollider(ramp, 0.1f, 0.1f, 0.0f);
        auto wallPos = ToWorld({-12.0f, s.halfThick + 1.0f}, s.origin, s.angle);
        auto wall = MakeStaticBox(sys, wallPos, s.angle, {0.2f, 1.0f});
        sys.SetCollider(wall, 0.1f, 0.1f, 0.0f);

        ColliderHandle bc;
        auto body = MakeBox(sys, {0.0f, 0.0f}, s.angle, s.boxHalf, true, &bc);
        sys.SetCollider(bc, 0.1f, 0.1f, 0.0f);
        sys.UpdateTransform(body, ToWorld({6.0f, s.halfThick + s.boxHalf}, s.origin, s.angle),
                            s.angle, {1.0f, 1.0f});

        float maxPen = 0.0f;
        for(int f = 0; f < 240; ++f)
        {
            sys.Step(DT);
            auto m = Measure(Snapshot(sys, body), s.origin, s.angle, s.halfThick, s.boxHalf);
            maxPen = std::max(maxPen, m.penetration);
        }
        auto m = Measure(Snapshot(sys, body), s.origin, s.angle, s.halfThick, s.boxHalf);
        Check(std::abs(m.vAlong) < 0.1f && m.along > -11.6f && m.along < -10.9f && maxPen < 0.05f,
              "slope/wall-stop", Fmt("along=%.3f vAlong=%.3f maxPen=%.3f",
                                     m.along, m.vAlong, maxPen));
    }

    // ============================ 平地回归 ============================
    void TestFallingBodies()
    {
        PhySystem sys(4, 8);
        MakeStaticBox(sys, {0.0f, -5.0f}, 0.0f, {100.0f, 0.5f});
        std::vector<RigidbodyHandle> bodies;
        for(int i = 0; i < 5; ++i)
            bodies.push_back(MakeBox(sys, {i * 1.1f, 0.0f}, 0.0f, 0.4f, true));
        float minY = 1e9f;
        for(int f = 0; f < 180; ++f)
        {
            sys.Step(DT);
            for(auto& b : bodies)
                minY = std::min(minY, Snapshot(sys, b).p.y);
        }
        bool ok = true;
        std::string detail;
        for(size_t i = 0; i < bodies.size(); ++i)
        {
            auto st = Snapshot(sys, bodies[i]);
            ok = ok && std::abs(st.p.y + 4.105f) < 0.02f;
            detail += Fmt(" y%zu=%.3f", i, st.p.y);
        }
        Check(ok && minY > -4.2f, "ground/falling", detail + Fmt(" minY=%.3f", minY));
    }

    void TestStack()
    {
        PhySystem sys(4, 8);
        MakeStaticBox(sys, {0.0f, -5.0f}, 0.0f, {100.0f, 0.5f});
        std::vector<RigidbodyHandle> bodies;
        for(int i = 0; i < 5; ++i)
            bodies.push_back(MakeBox(sys, {0.0f, i * 1.0f}, 0.0f, 0.4f, true));
        for(int f = 0; f < 300; ++f)
            sys.Step(DT);
        bool ok = true;
        std::string detail;
        for(size_t i = 0; i < bodies.size(); ++i)
        {
            float expect = -4.1f + i * 0.8f;
            auto st = Snapshot(sys, bodies[i]);
            ok = ok && std::abs(st.p.y - expect) < 0.06f;
            detail += Fmt(" y%zu=%.3f/%.2f", i, st.p.y, expect);
        }
        Check(ok, "ground/stack-5", detail);
    }

    void TestBullet()
    {
        PhySystem sys(4, 8);
        MakeStaticBox(sys, {0.0f, -5.0f}, 0.0f, {100.0f, 0.5f});
        auto b = MakeBox(sys, {0.0f, 5.0f}, 0.0f, 0.4f, true);
        sys.SetVelocity(b, {0.0f, -60.0f}, 0.0f);
        float minY = 1e9f;
        for(int f = 0; f < 120; ++f)
        {
            sys.Step(DT);
            minY = std::min(minY, Snapshot(sys, b).p.y);
        }
        auto st = Snapshot(sys, b);
        Check(std::abs(st.p.y + 4.105f) < 0.02f && minY > -4.3f, "ccd/bullet-60",
              Fmt("y=%.3f minY=%.3f", st.p.y, minY));
    }

    void TestFlatFriction()
    {
        PhySystem sys(4, 8);
        auto g = MakeStaticBox(sys, {0.0f, -5.0f}, 0.0f, {100.0f, 0.5f});
        sys.SetCollider(g, 0.6f, 0.5f, 0.0f);
        ColliderHandle bc;
        auto b = MakeBox(sys, {-10.0f, -4.1f}, 0.0f, 0.4f, true, &bc);
        sys.SetCollider(bc, 0.6f, 0.5f, 0.0f);
        sys.SetVelocity(b, {10.0f, 0.0f}, 0.0f);
        for(int f = 0; f < 180; ++f)
            sys.Step(DT);
        auto st = Snapshot(sys, b);
        Check(std::abs(st.v.x) < 0.1f && std::abs(st.p.y + 4.105f) < 0.02f, "friction/flat",
              Fmt("x=%.3f vx=%.3f y=%.3f", st.p.x, st.v.x, st.p.y));
    }
}

int main()
{
    TestKinematics();
    TestSlopeStatic(15.0f, 0.5f, 0.15f, "slope/static-15deg-mu0.5");
    TestSlopeStatic(30.0f, 0.8f, 0.20f, "slope/static-30deg-mu0.8");
    TestSlopeSlide(30.0f, 0.2f, 3.20f, 0.25f, "slope/slide-30deg-mu0.2");
    TestSlopeSlide(45.0f, 0.1f, 6.24f, 0.35f, "slope/slide-45deg-mu0.1");
    TestSlopeDrop();
    TestSlopeWall();
    TestFallingBodies();
    TestStack();
    TestBullet();
    TestFlatFriction();

    printf("\n==== %d passed, %d failed ====\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
