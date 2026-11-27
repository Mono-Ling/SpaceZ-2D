#pragma once
#include "Core/Math/Vector2.h"
#include "Core/Collision/Info/CollisionInfo.h"

namespace UnityPlugin
{
    struct CollisionInfo
    {
        float depth = -1.0f;
        Core::Math::Vector2 point;
        Core::Math::Vector2 normal;

        CollisionInfo() = default;
        CollisionInfo(const Core::Collision::CollisionInfo& core)
        : depth(core.depth), point(core.point), normal(core.normal) {}
    };
}