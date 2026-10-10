#include "CollisionDetection/Geometry/Bound.h"
#include "Math/Math.h"
using namespace Core::Math;

namespace Core::Collision
{
    Bound::Bound(const Vector2& center, const Vector2& extents) : center(center)
    {
        this->extents = Vector2(Abs(extents.x), Abs(extents.y));
    }
    Vector2 Bound::Max() const
    {
        return center + extents;
    }
    Vector2 Bound::Min() const
    {
        return center - extents;
    }
    float Bound::Area() const
    {
        return 4.0f * extents.x * extents.y;
    }

    Bound operator+(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto minB = b.Min();
        auto maxA = a.Max();
        auto maxB = b.Max();
        
        auto minV = Vector2
        {
            Min(minA.x, minB.x),
            Min(minA.y, minB.y)
        };
        auto maxV = Vector2
        {
            Max(maxA.x, maxB.x),
            Max(maxA.y, maxB.y)
        };
        return CreateBound(minV, maxV);
    }
    Bound& operator+=(Bound& a, const Bound& b)
    {
        a = a + b;
        return a;
    }
    bool operator==(const Bound& a, const Bound& b)
    {
        return a.center == b.center && a.extents == b.extents;
    }
    bool operator!=(const Bound& a, const Bound& b)
    {
        return !(a == b);
    }

    Bound CreateBound(const Math::Vector2& minV, const Math::Vector2& maxV)
    {
        auto center = (maxV - minV) / 2.0f + minV;
        auto extents = maxV - center;
        return Bound(center, extents);
    }
    bool IsIntersect(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto maxA = a.Max();
        auto minB = b.Min();
        auto maxB = b.Max();
        if (minA.x > maxB.x || minB.x > maxA.x)
            return false;
        if (minA.y > maxB.y || minB.y > maxA.y)
            return false;
        return true;
    }
    bool IsContains(const Bound& a, const Bound& b)
    {
        auto minA = a.Min();
        auto maxA = a.Max();
        auto minB = b.Min();
        auto maxB = b.Max();
        return maxA.x >= maxB.x && minA.x <= minB.x
            && maxA.y >= maxB.y && minA.y <= minB.y;
    }
}