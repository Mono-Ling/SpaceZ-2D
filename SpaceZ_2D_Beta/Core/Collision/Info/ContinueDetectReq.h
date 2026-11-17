#pragma once
#include "Core/Collision/Info/ConvexDetectReq.h"
#include "Core/Math/Vector2.h"
#include <utility>

namespace Core::Collision
{
    struct ContinueDetectReq
    {
        ConvexDetectReq first, second;
        std::pair<Math::Vector2, Math::Vector2> linearVelocity;
        std::pair<float, float> angularVelocity;

        inline explicit operator bool() const { return first && second; }
        inline void Step(float dt)
        {
            if(dt < 0)
                return;
            first.Step(linearVelocity.first * dt, angularVelocity.first * dt);
            second.Step(linearVelocity.second * dt, angularVelocity.second * dt);
        }
    };
}