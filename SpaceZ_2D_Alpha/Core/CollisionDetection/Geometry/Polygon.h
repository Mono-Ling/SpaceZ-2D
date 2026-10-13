#pragma once
#include "Math/Vector2.h"
#include "CollisionDetection/Geometry/Simplex.h"
#include <vector>

namespace Core::Collision
{
    struct Polygon
    {
    private:
        struct Line
        {
            bool used;
            int A;
            int B;
            float distance;
            Math::Vector2 normal;

            Line(int a, int b, const std::vector<Math::Vector2>& points);

            bool IsVisible(const Math::Vector2& point, const std::vector<Math::Vector2>& points);
        };

        std::vector<Math::Vector2> _points;
        std::vector<Line> _lines;
    
    public:
        Polygon(const Simplex& simplex);

        bool IsEnable() const;
        void GetMinDisToOriginLine(Math::Vector2& normal, float& depth) const;
        bool IsApproachBound(const Math::Vector2& newPoint) const;
        bool AddPoint(const Math::Vector2& newPoint);
        std::vector<Math::Vector2> GetPolygonPoints() const;
    
    private:
        void CompactLine();
        void RemoveVisibleLine();
        int GetMinDisToOriginLineIndex() const;
        void UpdateVisibleLine(const Math::Vector2& newPoint);
        std::vector<int> GetVisibleLineBound();
    };
}