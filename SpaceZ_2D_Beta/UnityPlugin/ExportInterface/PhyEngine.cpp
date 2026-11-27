#include "UnityPlugin/ExportInterface/ExportInterface.h"
#include "UnityPlugin/Profiler/ProfilerMarker.h"
#include "UnityPlugin/Debug/Debug.h"
#include "UnityPlugin/ExportData/NativeArray.h"
#include "UnityPlugin/ExportData/BodyState.h"
#include "UnityPlugin/ExportData/CollisionPair.h"
using namespace Core;
using namespace Core::Math;
using namespace std;

namespace UnityPlugin
{
    static ProfilerMarker g_phyStepMarker("【SpaceZ-2D】Step");

    EXPORT_FUNCTION void StartEngine(int subStep, int iteration)
    {
        if(g_phySystem)
            return;
        TRY_CATCH_DEBUG
        (
            {
                g_phySystem = new PhySystem(subStep, iteration);
            },
            "PhyEngine",
            "物理系统启动异常"
        );
    }
    EXPORT_FUNCTION void StopEngine()
    {
        if(!PhyAvailable())
            return;
        TRY_CATCH_DEBUG
        (
            {
                delete g_phySystem;
            },
            "PhyEngine",
            "物理系统关闭异常"
        );
        g_phySystem = nullptr;
    }
    EXPORT_FUNCTION void Step(float dt)
    {
        if(!PhyAvailable())
            return;

        TempProfilerMarker marker(g_phyStepMarker);
        TRY_CATCH_DEBUG
        (
            {
                g_phySystem->Step(dt);
            },
            "PhyEngine",
            "物理系统步进异常"
        );
    }
    EXPORT_FUNCTION NativeArray DirtyBodies()
    {
        if(!PhyAvailable())
            return {};
        TRY_CATCH_DEBUG
        (
            {
                int count = g_phySystem->GetDirtyBodyCount();
                if(count <= 0)
                    return {};

                NativeArray ans = CreateNativeArray<BodyState>(count);
                int curr = 0;
                g_phySystem->ForeachDirtyBodies(
                [&](const RigidbodyHandle& handle, const Vector2& pos, float rot, const Vector2& lv, float av)
                {
                    if(curr >= count)
                        return;

                    BodyState& state = At<BodyState>(ans, curr++);
                    state.handle = handle;
                    state.position = pos;
                    state.rotation = rot;
                    state.linearVelocity = lv;
                    state.angularVelocity = av;
                });
                ans.size = curr;
                return ans;
            },
            "PhyEngine",
            "获取脏刚体异常"
        );
        return {};
    }
    EXPORT_FUNCTION NativeArray EnterCollisionPairs()
    {
        if(!PhyAvailable())
            return {};
        TRY_CATCH_DEBUG
        (
            {
                int count = g_phySystem->GetEnterCollisionPairsCount();
                if(count <= 0)
                    return {};

                NativeArray ans = CreateNativeArray<CollisionPair>(count);
                int curr = 0;
                g_phySystem->ForeachEnterPairs([&](const Core::CollisionObj& obj, const vector<Core::Collision::CollisionInfo>& infos)
                {
                    if(curr >= count)
                        return;
                    At<CollisionPair>(ans, curr++) = CollisionPair(obj, infos);
                });
                return ans;
            },
            "PhyEngine",
            "获取初始碰撞对异常"
        );
        return {};
    }
    EXPORT_FUNCTION NativeArray StayCollisionPairs()
    {
        if(!PhyAvailable())
            return {};
        TRY_CATCH_DEBUG
        (
            {
                int count = g_phySystem->GetStayCollisionPairsCount();
                if(count <= 0)
                    return {};

                NativeArray ans = CreateNativeArray<CollisionPair>(count);
                int curr = 0;
                g_phySystem->ForeachStayPairs([&](const Core::CollisionObj& obj, const vector<Core::Collision::CollisionInfo>& infos)
                {
                    if(curr >= count)
                        return;
                    At<CollisionPair>(ans, curr++) = CollisionPair(obj, infos);
                });
                return ans;
            },
            "PhyEngine",
            "获取持续碰撞对异常"
        );
        return {};
    }
    EXPORT_FUNCTION NativeArray ExitPairs()
    {
        if(!PhyAvailable())
            return {};
        TRY_CATCH_DEBUG
        (
            {
                int count = g_phySystem->GetExitCollisionPairsCount();
                if(count <= 0)
                    return {};

                NativeArray ans = CreateNativeArray<CollisionObj>(count);
                int curr = 0;
                g_phySystem->ForeachExitPairs([&](const Core::CollisionObj& obj)
                {
                    if(curr >= count)
                        return;
                    At<CollisionObj>(ans, curr++) = CollisionObj(obj);
                });
                return ans;
            },
            "PhyEngine",
            "获取结束碰撞对异常"
        );
        return {};
    }
}
