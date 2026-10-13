#pragma once
#include "Math/Vector2.h"
#include <array>

namespace Core::Collision
{
    struct Simplex
    {
        int currentCount = 0;
        std::array<Math::Vector2, 3> points;

        bool AddPoint(const Math::Vector2& point);
        bool ContainsOrigin(Math::Vector2& update);

        bool TryComplete(Math::Vector2& update);

    private:
        bool PointContainsOrigin(Math::Vector2& update);
        bool LineContainsOrigin(Math::Vector2& update);
        bool TriangleContainsOrigin(Math::Vector2& update);
        void SetPoints(const Math::Vector2& point);
        void SetPoints(const Math::Vector2& a, const Math::Vector2& b);
        Math::Vector2 GetVerticalToOrigin(const Math::Vector2& a, const Math::Vector2& b);

        void ClearCollinear();
    };
}