#pragma once
#include "Math/Vector2.h"
#include <utility>

namespace Core::Collision
{
    struct CollisionInfo
    {
        float depth = 0.0f;
        Math::Vector2 normal;
        Math::Vector2 point;
    };
}