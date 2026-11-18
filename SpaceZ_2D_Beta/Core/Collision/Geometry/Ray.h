#pragma once
#include "Core/Math/Vector2.h"
#include "Core/Collision/Geometry/Bound.h"
#include "Core/Math/Math.h"
#include <cfloat>
#include <utility>

namespace Core::Collision
{
    struct Ray
    {
        float maxDis = 0.0f;
        Math::Vector2 origin;
        Math::Vector2 direction;

        Ray(const Math::Vector2& o, const Math::Vector2& dir, float maxDis)
        : origin(o), direction(dir.Normalized()), maxDis(Math::Max(maxDis, 0.0f)) {}

        inline explicit operator bool() const { return direction != Math::Vector2::zero; }
        inline bool IsIntersect(const Bound& bound)
        {
            const Math::Vector2 bMin = bound.Min();
            const Math::Vector2 bMax = bound.Max();

            float tXMin, tXMax;
            float tYMin, tYMax;

            // X slab
            if (Math::Abs(direction.x) < Math::Epsilon)
            {
                if (origin.x < bMin.x || origin.x > bMax.x)
                    return false;
                tXMin = -FLT_MAX;
                tXMax = FLT_MAX;
            }
            else
            {
                float invDx = 1.0f / direction.x;
                tXMin = (bMin.x - origin.x) * invDx;
                tXMax = (bMax.x - origin.x) * invDx;
                if (tXMin > tXMax) std::swap(tXMin, tXMax);
            }

            // Y slab
            if (Math::Abs(direction.y) < Math::Epsilon)
            {
                if (origin.y < bMin.y || origin.y > bMax.y)
                    return false;
                tYMin = -FLT_MAX;
                tYMax = FLT_MAX;
            }
            else
            {
                float invDy = 1.0f / direction.y;
                tYMin = (bMin.y - origin.y) * invDy;
                tYMax = (bMax.y - origin.y) * invDy;
                if (tYMin > tYMax) std::swap(tYMin, tYMax);
            }

            float tNear = Math::Max(tXMin, tYMin);
            float tFar  = Math::Min(tXMax, tYMax);

            if (tNear > tFar)
                return false;

            float tEnter = Math::Max(tNear, 0.0f);
            return tEnter <= tFar && tEnter <= maxDis;
        }
    };
}