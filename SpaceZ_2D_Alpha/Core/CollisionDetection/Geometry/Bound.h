#pragma once
#include "Math/Vector2.h"

namespace Core::Collision
{
    struct Bound
    {
        Math::Vector2 center = Math::Vector2::zero;
        Math::Vector2 extents = Math::Vector2::one;

        Bound(const Math::Vector2& center, const Math::Vector2& extents);

        Math::Vector2 Max() const;
        Math::Vector2 Min() const;
        float Area() const;
    };
    Bound operator+(const Bound& a, const Bound& b);
    Bound& operator+=(Bound& a, const Bound& b);
    bool operator==(const Bound& a, const Bound& b);
    bool operator!=(const Bound& a, const Bound& b);

    Bound CreateBound(const Math::Vector2& minV, const Math::Vector2& maxV);
    bool IsIntersect(const Bound& a, const Bound& b);
    bool IsContains(const Bound& a, const Bound& b);
}
