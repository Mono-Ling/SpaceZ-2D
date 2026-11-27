#pragma once
#include "Core/System/Info/CollisionPair.h"
#include "UnityPlugin/ExportData/CollisionObj.h"
#include "UnityPlugin/ExportData/NativeArray.h"
#include "UnityPlugin/ExportData/CollisionInfo.h"

namespace UnityPlugin
{
    struct CollisionPair
    {
        CollisionObj collision;
        NativeArray infos;

        CollisionPair() = default;
        CollisionPair(const Core::CollisionObj& obj, const std::vector<Core::Collision::CollisionInfo>& coreInfos)
        : collision(obj)
        {
            infos = CreateNativeArray<CollisionInfo>(static_cast<int>(coreInfos.size()));
            for(int i = 0; i < coreInfos.size(); i++)
                At<CollisionInfo>(infos, i) = coreInfos[i];
        }
    };
}