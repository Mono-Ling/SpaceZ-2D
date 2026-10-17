#include "ExportInterface/ExportInterface.h"
#include "ExportInterface/ExportInfo/ExportCollisionPair.h"
#include "ExportInterface/ExportInfo/ExportBodyInfo.h"
#include "Math/Math.h"
using namespace Core;
using namespace Core::Math;
using namespace Core::Collision;
using namespace Export;

PhySystem* phySystemPtr = nullptr;

EXPORT_FUNCTION void StartEngine(int subStep, int iteration)
{
    if(phySystemPtr)
    {
        Debug::LogWarning("【引擎生命周期】引擎重复启动");
        return;
    }
    TRY_CATCH_DEBUG
    (
        { phySystemPtr = new PhySystem(subStep, iteration); },
        "引擎生命周期",
        "物理引擎启动异常"
    );
}
EXPORT_FUNCTION void StopEngine()
{
    if(!phySystemPtr)
        return;
    TRY_CATCH_DEBUG
    (
        {
            delete phySystemPtr;
            phySystemPtr = nullptr;
        },
        "引擎生命周期",
        "物理引擎关闭异常"
    );
}
EXPORT_FUNCTION void EngineStep(float dt)
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        { phySystemPtr->Step(dt); },
        "引擎生命周期",
        "引擎步进异常"
    );
}
EXPORT_FUNCTION int GetCollisionCount()
{
    if(!IsEngineAvailable())
        return -1;
    TRY_CATCH_DEBUG
    (
        { return phySystemPtr->GetCollisionPairCount(); },
        "引擎生命周期",
        "获取碰撞对数量异常"
    );
    return -1;
}
EXPORT_FUNCTION void GetCollisionPairs(ExportCollisionPair* pairs, int& count)
{
    if(!IsEngineAvailable() || count <= 0 || !pairs)
    {
        count = -1;
        return;
    }
    TRY_CATCH_DEBUG
    (
        {
            count = Min(count, phySystemPtr->GetCollisionPairCount());
            int i = 0;
            phySystemPtr->ForeachCollisionPairs([&](const CollisionPair& p)
            {
                if(i >= count)
                    return;
                pairs[i++] = ExportCollisionPair::From(p);
            });
            count = i;
            return;
        },
        "引擎生命周期",
        "获取碰撞对异常"
    );
    count = -1;
}
EXPORT_FUNCTION int GetDirtyBodyCount()
{
    if(!IsEngineAvailable())
        return -1;
    TRY_CATCH_DEBUG
    (
        { return phySystemPtr->GetDirtyBodyCount(); },
        "引擎生命周期",
        "获取更改刚体数量异常"
    );
    return -1;
}
EXPORT_FUNCTION void GetDirtyBody(ExportBodyInfo* infos, int& count)
{
    if(!IsEngineAvailable() || count < 0 || !infos)
    {
        count = -1;
        return;
    }
    TRY_CATCH_DEBUG
    (
        {
            count = Min(count, phySystemPtr->GetDirtyBodyCount());
            int i = 0;
            phySystemPtr->ForeachDirtyBodies(
                [&](const RigidbodyHandle& handle, const Vector2& pos, float rot, const Vector2& lv, float av)
            {
                if(i >= count)
                    return;
                infos[i++] = ExportBodyInfo::From(handle, pos, rot, lv, av);
            });
            count = i;
            return;
        },
        "引擎生命周期",
        "获取更改刚体异常"
    );
    count = -1;
}