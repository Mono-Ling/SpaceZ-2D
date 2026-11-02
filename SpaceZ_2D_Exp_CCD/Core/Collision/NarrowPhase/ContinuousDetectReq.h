#pragma once
#include "Collision/NarrowPhase/ConvexDetectReq.h"
#include "Math/Vector2.h"
#include <utility>

namespace Core::Collision
{
    struct ContinuousDetectReq
    {
        ConvexDetectReq first;
        ConvexDetectReq second;
        std::pair<Math::Vector2, Math::Vector2> velocity;
        std::pair<float, float> angularVelocity;

        ContinuousDetectReq(const ConvexDetectReq& first, const ConvexDetectReq& second, const std::pair<Math::Vector2, Math::Vector2>& velocity, const std::pair<float, float>& angularVelocity)
            : first(first), second(second), velocity(velocity), angularVelocity(angularVelocity) {}

        inline explicit operator bool() const
        {
            return first && second;
        }
        inline void Step(float dt)
        {
            if(dt <= 0.0f)
                return;
            first.Step(velocity.first * dt, angularVelocity.first * dt);
            second.Step(velocity.second * dt, angularVelocity.second * dt);
        }
    };
}