#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/System/Info/CollisionObj.h"
#include "Core/System/Info/CollisionPair.h"

namespace UnityPlugin
{
    struct CollisionObj
    {
        Core::ColliderHandle firstCollider;
        Core::RigidbodyHandle firstBody;

        Core::ColliderHandle secondCollider;
        Core::RigidbodyHandle secondBody;

        CollisionObj() = default;
        CollisionObj(const Core::CollisionObj& core)
        : firstCollider(core.colliders.first), firstBody(core.bodies.first),
          secondCollider(core.colliders.second), secondBody(core.bodies.second) {}
    };
}