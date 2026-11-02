#pragma once
#include "Math/Vector2.h"

namespace Core::Collision
{
    struct Bound
    {
        Math::Vector2 center;
        Math::Vector2 extents;

        Bound() = default;
        Bound(const Math::Vector2& center, const Math::Vector2& extents)
        : center(center), extents(extents) {}

        Math::Vector2 Max() const;
        Math::Vector2 Min() const;
        void SetBound(const Math::Vector2& min, const Math::Vector2& max);

        float Area() const;
    };
    bool operator==(const Bound& a, const Bound& b);
    bool operator!=(const Bound& a, const Bound& b);
    Bound operator+(const Bound& a, const Bound& b);
    Bound& operator+=(Bound& a, const Bound& b);

    bool IsIntersect(const Bound& a, const Bound& b);
    bool IsContains(const Bound& a, const Bound& b);
}