#pragma once
#include "Core/Math/Vector2.h"

namespace Core::Collision
{
    struct CollisionInfo
    {
        float depth = 0.0f;
        Math::Vector2 point;
        Math::Vector2 normal;
    };
}