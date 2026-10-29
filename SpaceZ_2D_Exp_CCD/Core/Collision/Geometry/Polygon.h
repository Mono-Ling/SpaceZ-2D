#pragma once
#include "Math/Vector2.h"
#include <vector>
#include <array>

namespace Core::Collision
{
    struct Line
    {
        bool used;
        int A, B;
        float distance;
        Math::Vector2 normal;

        Line(int a, int b, const std::vector<Math::Vector2>& points);

        bool IsVisible(const Math::Vector2& point, const std::vector<Math::Vector2>& points) const;
    };
    
    struct Polygon
    {
        std::vector<Math::Vector2> points;
        std::vector<Line> lines;

        Polygon(const std::array<Math::Vector2, 3>& arr, int count);

        explicit operator bool() const;
        void GetMinDisToOriginLine(Math::Vector2& normal, float& depth) const;
        bool IsApproachBound(const Math::Vector2& newPoint) const;
        void AddPoint(const Math::Vector2& newPoint);

    private:
        void RemoveVisibleLine();
        void UpdateVisibleLine(const Math::Vector2& newPoint);
        int GetMinDisToOriginLineIndex() const;
        std::vector<int> GetVisibleBound();
    };
}