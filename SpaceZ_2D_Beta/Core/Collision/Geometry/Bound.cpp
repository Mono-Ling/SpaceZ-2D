#include "Core/Collision/Geometry/Bound.h"
using namespace Core::Math;

namespace Core::Collision
{
    Vector2 Bound::Min() const
    {
        return center - extents;
    }
    Vector2 Bound::Max() const
    {
        return center + extents;
    }
    float Bound::Area() const
    {
        return 4.0f * extents.x * extents.y;
    }

    Bound Bound::From(const Vector2& min, const Vector2& max)
    {
        Vector2 realMin = Math::Min(min, max);
        Vector2 realMax = Math::Max(min, max);

        auto center = (realMin + realMax) / 2.0f;
        return Bound
        {
            center,
            realMax - center,
        };
    }

    bool operator==(const Bound& a, const Bound& b)
    {
        return a.center == b.center && a.extents == b.extents;
    }
    bool operator!=(const Bound& a, const Bound& b)
    {
        return !(a == b);
    }
    Bound operator+(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto minB = b.Min();
        auto maxA = a.Max();
        auto maxB = b.Max();
        return Bound::From(Min(minA, minB), Max(maxA, maxB));
    }
    Bound& operator+=(Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto minB = b.Min();
        auto maxA = a.Max();
        auto maxB = b.Max();

        auto min = Min(minA, minB);
        auto max = Max(maxA, maxB);
        a.center = (min + max) / 2.0f;
        a.extents = max - a.center;

        return a;
    }

    bool IsIntersect(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto minB = b.Min();
        auto maxA = a.Max();
        auto maxB = b.Max();

        if(minA.x > maxB.x || minB.x > maxA.x)
            return false;
        if(minA.y > maxB.y || minB.y > maxA.y)
            return false;
        return true;
    }
    bool IsContains(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto minB = b.Min();
        auto maxA = a.Max();
        auto maxB = b.Max();

        return minA.x <= minB.x && maxB.x <= maxA.x
            && minA.y <= minB.y && maxB.y <= maxA.y;
    }
}