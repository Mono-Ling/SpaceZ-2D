#pragma once
#include "Math/Vector2.h"
#include <array>

namespace Core::Collision
{
    struct Simplex
    {
        int count = 0;
        std::array<Math::Vector2, 3> points;

        bool AddPoint(const Math::Vector2& point);
        bool ContainsOrigin(Math::Vector2& update);

        bool TryComplete(Math::Vector2& update);
        
    private:
        void SetPoints(const Math::Vector2& a, const Math::Vector2& b);
        void SetPoint(const Math::Vector2& point);
        void ClearCollinear();
        Math::Vector2 GetVerticalToOrigin(const Math::Vector2& a, const Math::Vector2& b);

        bool PointContainsOrigin(Math::Vector2& update);
        bool LineContainsOrigin(Math::Vector2& update);
        bool TriangleContainsOrigin(Math::Vector2& update);
    };
}