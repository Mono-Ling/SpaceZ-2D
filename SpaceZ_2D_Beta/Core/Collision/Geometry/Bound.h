#pragma once
#include "Core/Math/Vector2.h"

namespace Core::Collision
{
    struct Bound
    {
        Math::Vector2 center;
        Math::Vector2 extents;

        Bound() = default;
        Bound(const Math::Vector2& cen, const Math::Vector2& ext)
        : center(cen), extents(ext) {}

        Math::Vector2 Min() const;
        Math::Vector2 Max() const;
        float Area() const;

        static Bound From(const Math::Vector2& min, const Math::Vector2& max);
    };
    bool operator==(const Bound& a, const Bound& b);
    bool operator!=(const Bound& a, const Bound& b);
    Bound operator+(const Bound& a, const Bound& b);
    Bound& operator+=(Bound& a, const Bound& b);

    bool IsIntersect(const Bound& a, const Bound& b);
    bool IsContains(const Bound& a, const Bound& b);
}