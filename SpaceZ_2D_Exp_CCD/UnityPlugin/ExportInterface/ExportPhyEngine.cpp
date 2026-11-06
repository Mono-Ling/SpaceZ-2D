#include "ExportInterface/ExportInterface.h"
#include "ExportInterface/ExportData/ExportObjInfo.h"
#include "ExportInterface/ExportData/ExportBodyInfo.h"
using namespace Core;
using namespace Core::Math;
using namespace ExportData;

PhySystem* phySystem = nullptr;

bool IsEngineAvailable()
{
    if(phySystem)
        return true;
    ::Debug::LogWarning("【物理引擎】引擎未启动");
    return false;
}

EXPORT_FUNCTION void StartEngine(int subStep, int iteration)
{
    if(phySystem)
    {
        ::Debug::LogWarning("【物理引擎】引擎重复启动");
        return;
    }
    TRY_CATCH_DEBUG
    (
        { phySystem = new PhySystem(subStep, iteration); },
        "物理引擎",
        "引擎启动异常"
    );
}
EXPORT_FUNCTION void StopEngine()
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        {
            delete phySystem;
            phySystem = nullptr;
        },
        "物理引擎",
        "引擎关闭异常"
    );
}
EXPORT_FUNCTION void Step(float dt)
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        {
            phySystem->Step(dt);
        },
        "物理引擎",
        "引擎步进异常"
    );
}
EXPORT_FUNCTION int GetDirtyBodyCount()
{
    if(!IsEngineAvailable())
        return -1;
    TRY_CATCH_DEBUG
    (
        {
            return phySystem->GetDirtyBodyCount();
        },
        "物理引擎",
        "更改刚体数量获取异常"
    );
    return -1;
}
EXPORT_FUNCTION void GetDirtyBodies(ExportObjInfo<Rigidbody, ExportBodyInfo>* infos, int& count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    
    TRY_CATCH_DEBUG
    (
        {
            int curr = 0;
            phySystem->ForeachDirtyBodies(
            [&](const RigidbodyHandle& handle, const Vector2& pos, float rot, const Vector2& lv, float av)
            {
                if(curr >= count)
                    return;
                auto& exportInfo = infos[curr++];
                exportInfo.handle = handle;
                exportInfo.info.position = pos;
                exportInfo.info.rotation = rot;
                exportInfo.info.linearVelocity = lv;
                exportInfo.info.angularVelocity = av;
            });
            count = curr;
        },
        "物理引擎",
        "更改刚体获取异常"
    );
}