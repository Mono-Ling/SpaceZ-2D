#include "Collision/Geometry/Simplex.h"
#include "Debug/Debug.h"
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    bool IsPointCountCorrect(int count, int target)
    {
        if(count == target)
            return true;
        Debug::LogError("【单纯形】顶点数量不匹配");
        return false;
    }

    bool Simplex::AddPoint(const Vector2& point)
    {
        if(count >= 3)
        {
            Debug::LogError("【单纯形】顶点溢出，添加顶点失败");
            return false;
        }
        points[count++] = point;
        if(count == 3)
            return !IsCollinear(points[0], points[1], points[2]);
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
        Debug::LogError("【单纯形】单纯形顶点数量无效");
        return false;
    }
    bool Simplex::TryComplete(Vector2& update)
    {
        ClearCollinear();
        if(count == 3)
            return true;
        if(count == 1)
        {
            update = -points[0];
            return false;
        }
        if(count == 2)
        {
            update = GetVerticalToOrigin(points[0], points[1]);
            return false;
        }
        Debug::LogError("【单纯形】单纯形顶点数量无效");
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
        auto A = points[0];
        auto B = points[1];
        auto C = points[2];
        float ABlen = (B - A).SqrLength();
        float AClen = (C - A).SqrLength();
        float BClen = (C - B).SqrLength();
        float maxLen = Max(ABlen, Max(AClen, BClen));

        if(Near(maxLen, ABlen))
            SetPoints(A, B);
        else if(Near(maxLen, AClen))
            SetPoints(A, C);
        else
            SetPoints(B, C);
    }
    Vector2 Simplex::GetVerticalToOrigin(const Vector2& a, const Vector2& b)
    {
        auto vertical = (b - a).Perp();
        return Dot(vertical, -a) < 0 ? -vertical : vertical;
    }
    bool Simplex::PointContainsOrigin(Vector2& update)
    {
        if(!IsPointCountCorrect(count, 1))
            return false;
        update = -points[0];
        return points[0] == Vector2::zero;
    }
    bool Simplex::LineContainsOrigin(Vector2& update)
    {
        if(!IsPointCountCorrect(count, 2))
            return false;
        auto A = points[0];
        auto B = points[1];
        auto AO = -A;
        auto BO = -B;
        auto AB = B - A;
        if(Dot(AO, AB) < 0)
        {
            update = -A;
            SetPoint(A);
            return false;
        }
        if(Dot(BO, -AB) < 0)
        {
            update = -B;
            SetPoint(B);
            return false;
        }
        update = GetVerticalToOrigin(A, B);
        return IsCollinear(A, B, Vector2::zero);
    }
    bool Simplex::TriangleContainsOrigin(Vector2& update)
    {
        if(!IsPointCountCorrect(count, 3))
            return false;
        auto A = points[0];
        auto B = points[1];
        auto C = points[2];
        auto ABn = GetVerticalToOrigin(A, B);
        auto ACn = GetVerticalToOrigin(A, C);
        auto BCn = GetVerticalToOrigin(B, C);
        if(Dot(ABn, C - A) < 0)
        {
            update = ABn;
            SetPoints(A, B);
            return false;
        }
        if(Dot(ACn, B - A) < 0)
        {
            update = ACn;
            SetPoints(A, C);
            return false;
        }
        if(Dot(BCn, A - B) < 0)
        {
            update = BCn;
            SetPoints(B, C);
            return false;
        }
        return true;
    }
}