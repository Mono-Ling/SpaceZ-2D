#include "Core/Collision/Geometry/Simplex.h"
#include "Core/Math/Math.h"
#include "Core/Debug/DebugFormat.h"
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    bool CorrectPointsCount(int count, int target)
    {
        if(count != target)
            Debug::LogWarningFormat("【单纯形】顶点数量无效，Target:", target, "Now:", count);
        return count == target;
    }

    bool Simplex::AddPoint(const Vector2& point)
    {
        if(count >= 3)
        {
            Debug::LogError("【Simplex】顶点溢出");
            return false;
        }
        points[count++] = point;
        if(count == 3)
            if(IsCollinear(points[0], points[1], points[2]))
            {
                ClearCollinear();
                return false;
            }
        return true;
    }
    bool Simplex::ContainsOrigin(Vector2& update)
    {
        switch(count)
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
    float Simplex::SeparateDis()
    {
        ClearCollinear();
        switch(count)
        {
        case 1:
            return Distance(points[0], Vector2::zero);

        case 2:
            return PointToLineSegDistance(Vector2::zero, points[0], points[1]);

        case 3:
            auto& A = points[0];
            auto& B = points[1];
            auto& C = points[2];
            auto ABn = GetVerticalToOrigin(A, B);
            auto ACn = GetVerticalToOrigin(A, C);
            auto BCn = GetVerticalToOrigin(B, C);
            if(Dot(ABn, C - A) >= 0 && Dot(ACn, B - A) >= 0 && Dot(BCn, A - B) >= 0)
                break;
            float ABdis = PointToLineSegDistance(Vector2::zero, A, B);
            float ACdis = PointToLineSegDistance(Vector2::zero, A, C);
            float BCdis = PointToLineSegDistance(Vector2::zero, B, C);
            return Min(Min(ABdis, ACdis), BCdis);
        }
        return -1.0f;
    }
    bool Simplex::TryComplete(Vector2& update)
    {
        switch(count)
        {
        case 1:
            update = -points[0];
        break;

        case 2:
            update = GetVerticalToOrigin(points[0], points[1]);
        break;

        case 3:
            auto& A = points[0];
            auto& B = points[1];
            auto& C = points[2];
            if(IsCollinear(A, B, Vector2::zero))
            {
                update = GetVerticalToOrigin(A, C);
                SetPoints(A, C);
                break;
            }
            if(IsCollinear(A, C, Vector2::zero))
            {
                update = GetVerticalToOrigin(A, B);
                SetPoints(A, B);
                break;
            }
            if(IsCollinear(B, C, Vector2::zero))
            {
                update = GetVerticalToOrigin(A, B);
                SetPoints(A, B);
                break;
            }
            return true;
        }
        return false;
    }

    void Simplex::SetPoints(const Vector2& a, const Vector2& b)
    {
        points[0] = a;
        points[1] = b;
        count = 2;
    }
    void Simplex::SetPoint(const Vector2& point)
    {
        points[0] = point;
        count = 1;
    }
    void Simplex::ClearCollinear()
    {
        if(count < 3)
            return;
        if(!IsCollinear(points[0], points[1], points[2]))
            return;
        auto& A = points[0];
        auto& B = points[1];
        auto& C = points[2];
        float ABlen = (B - A).SqrLength();
        float AClen = (C - A).SqrLength();
        float BClen = (C - B).SqrLength();
        float maxLen = Max(Max(ABlen, AClen), BClen);

        if(Near(ABlen, maxLen))
            SetPoints(A, B);
        else if(Near(AClen, maxLen))
            SetPoints(A, C);
        else
            SetPoints(B, C);
    }
    Vector2 Simplex::GetVerticalToOrigin(const Vector2& a, const Vector2& b)
    {
        if(a == b)
            return Vector2::zero;
        auto normal = (b - a).Perp();
        return Dot(normal, -a) < 0 ? -normal : normal;
    }
    bool Simplex::PointContainsOrigin(Vector2& update)
    {
        if(!CorrectPointsCount(count, 1))
            return false;
        update = -points[0];
        return false;
    }
    bool Simplex::LineContainsOrigin(Vector2& update)
    {
        if(!CorrectPointsCount(count, 2))
            return false;
        auto& A = points[0];
        auto& B = points[1];
        if(Dot(-A, B - A) < 0)
        {
            SetPoint(A);
            update = -A;
            return false;
        }
        if(Dot(-B, A - B) < 0)
        {
            SetPoint(B);
            update = -B;
            return false;
        }
        update = GetVerticalToOrigin(A, B);
        return false;
    }
    bool Simplex::TriangleContainsOrigin(Vector2& update)
    {
        if(!CorrectPointsCount(count, 3))
            return false;
        auto& A = points[0];
        auto& B = points[1];
        auto& C = points[2];
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
}