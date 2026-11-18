#pragma once
#include "Core/Math/Vector2.h"

namespace Core::Collision
{
    struct RayHit
    {
        Math::Vector2 point;
        Math::Vector2 normal;
        float distance = -1.0f;

        inline explicit operator bool() const { return distance >= 0.0f; }
    };
}