#include "Collision/Geometry/Bound.h"
#include "Math/Math.h"
using namespace Core::Math;

namespace Core::Collision
{
    Vector2 Bound::Max() const
    {
        return center + extents;
    }
    Vector2 Bound::Min() const
    {
        return center - extents;
    }
    void Bound::SetBound(const Vector2& min, const Vector2& max)
    {
        Vector2 realMin(Math::Min(min.x, max.x), Math::Min(min.y, max.y));
        Vector2 realMax(Math::Max(min.x, max.x), Math::Max(min.y, max.y));
        center = (realMin + realMax) / 2.0f;
        extents = realMax - center;
    }
    float Bound::Area() const { return 4.0f * extents.x * extents.y; }

    bool operator==(const Bound& a, const Bound& b)
    {
        return a.center == b.center && a.extents == b.extents;
    }
    bool operator!=(const Bound& a, const Bound& b) { return !(a == b); }
    Bound operator+(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto maxA = a.Max();
        auto minB = b.Min();
        auto maxB = b.Max();
        Vector2 realMin
        {
            Min(minA.x, minB.x),
            Min(minA.y, minB.y),
        };
        Vector2 realMax
        {
            Max(maxA.x, maxB.x),
            Max(maxA.y, maxB.y),
        };
        Bound ans;
        ans.SetBound(realMin, realMax);
        return ans;
    }
    Bound& operator+=(Bound& a, const Bound& b)
    {
        a = a + b;
        return a;
    }
    bool IsIntersect(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto maxA = a.Max();
        auto minB = b.Min();
        auto maxB = b.Max();
        bool x = maxA.x < minB.x || minA.x > maxB.x;
        bool y = maxA.y < minB.y || minA.y > maxB.y;
        return !x && !y;
    }
    bool IsContains(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto maxA = a.Max();
        auto minB = b.Min();
        auto maxB = b.Max();
        return minA.x <= minB.x && maxA.x >= maxB.x
            && minA.y <= minB.y && maxA.y >= maxB.y;
    }
}