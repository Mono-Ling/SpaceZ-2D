#pragma once
#include "Core/Math/Vector2.h"
#include <vector>
#include <array>
#include <unordered_map>

namespace Core::Collision
{
    struct Polygon
    {
    private:
        struct Line
        {
            bool used;
            int A = 0;
            int B = 0;
            float distance = 0.0f;
            Math::Vector2 normal;

            Line(int a, int b, const std::vector<Math::Vector2>& points);

            bool Visible(const Math::Vector2& newPoint, const std::vector<Math::Vector2>& points);
        };

        std::vector<Math::Vector2> _points;
        std::vector<Line> _lines;

        std::unordered_map<int, int> _visiblePoints;

    public:
        Polygon(const std::array<Math::Vector2, 3>& arr);

        explicit operator bool() const;
        void MinLineToOriginDis(Math::Vector2& normal, float& depth) const;
        bool IsApproachBound(const Math::Vector2& point) const;
        void AddPoint(const Math::Vector2& point);

    private:
        int GetMinLineToOriginDisIndex() const;
        void SignVisiblePoint(const Math::Vector2& newPoint);
        std::vector<int> VisibleBound();
        void RemoveVisibleLine();
    };
}