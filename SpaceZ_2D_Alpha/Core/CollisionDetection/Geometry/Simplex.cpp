#include "CollisionDetection/Geometry/Simplex.h"
#include "Math/Math.h"
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    bool Simplex::AddPoint(const Vector2& point)
    {
        if(currentCount == 3)
            return false;
        points[currentCount++] = point;
        return true;
    }
    bool Simplex::ContainsOrigin(Vector2& update)
    {
        switch(currentCount)
        {
            case 1:
                return PointContainsOrigin(update);
            case 2:
                return LineContainsOrigin(update);
            case 3:
                return TriangleContainsOrigin(update);
        }
        return false;
    }
    bool Simplex::TryComplete(Math::Vector2& update)
    {
        ClearCollinear();
        if(currentCount == 3)
            return true;
        if(currentCount == 1)
        {
            update = -points[0];
            return false;
        }
        update = GetVerticalToOrigin(points[0], points[1]);
        return false;
    }

    bool Simplex::PointContainsOrigin(Vector2& update)
    {
        if(currentCount != 1)
            return false;
        update = -points[0];
        return points[0] == Vector2::zero;
    }
    bool Simplex::LineContainsOrigin(Vector2& update)
    {
        if(currentCount != 2)
            return false;
        auto A = points[1];
        auto B = points[0];
        auto AB = B - A;
        auto AO = -A;
        if(Dot(AB, AO) < 0)
        {
            SetPoints(A);
            update = -A;
        }
        else
            update = GetVerticalToOrigin(A, B);
        return false;
    }
    bool Simplex::TriangleContainsOrigin(Vector2& update)
    {
        if(currentCount != 3)
            return false;
        auto A = points[2];
        auto B = points[1];
        auto C = points[0];
        auto ABn = GetVerticalToOrigin(A, B);
        auto ACn = GetVerticalToOrigin(A, C);
        auto BCn = GetVerticalToOrigin(B, C);
        if(Dot(ABn, C - A) < 0)
        {
            SetPoints(A, B);
            update = ABn;
            return false;
        }
        if(Dot(ACn, B - A) < 0)
        {
            SetPoints(A, C);
            update = ACn;
            return false;
        }
        if(Dot(BCn, A - B) < 0)
        {
            SetPoints(B, C);
            update = BCn;
            return false;
        }
        return true;
    }

    void Simplex::SetPoints(const Vector2& point)
    {
        points[0] = point;
        currentCount = 1;
    }
    void Simplex::SetPoints(const Vector2& a, const Vector2& b)
    {
        points[0] = a;
        points[1] = b;
        currentCount = 2;
    }
    Vector2 Simplex::GetVerticalToOrigin(const Vector2& a, const Vector2& b)
    {
        auto v = (b - a).Perp();
        return Dot(v, -a) < 0 ? -v : v;
    }
    void Simplex::ClearCollinear()
    {
        
        auto A = points[2];
        auto B = points[1];
        auto C = points[0];
        if(!IsCollinear(A, B, C))
            return;

        float ABlen = (B - A).LengthSquared();
        float AClen = (C - A).LengthSquared();
        float BClen = (C - B).LengthSquared();
        float maxLen = Max(Max(ABlen, AClen), BClen);

        if(Abs(ABlen - maxLen) < Epsilon * Epsilon)
            SetPoints(A, B);
        else if(Abs(AClen - maxLen) < Epsilon * Epsilon)
            SetPoints(A, C);
        else
            SetPoints(B, C);
    }
}