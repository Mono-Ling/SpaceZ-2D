#include "CollisionDetection/Geometry/Polygon.h"
#include "Math/Math.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cfloat>
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    Polygon::Line::Line(int a, int b, const vector<Vector2>& points) : A(a), B(b), used(true)
    {
        normal = (points[b] - points[a]).Perp().Normalized();
        if(Dot(points[a], normal) < 0)
            normal = -normal;
        distance = PointToLineDistance(Vector2::zero, points[a], points[b]);
    }
    bool Polygon::Line::IsVisible(const Vector2& point, const vector<Vector2>& points)
    {
        return Dot(point - points[A], normal) > 0;
    }

    Polygon::Polygon(const Simplex& simplex)
    {
        _points = vector<Vector2>(simplex.points.begin(), simplex.points.end());
        if(_points.size() < 3)
            return;
        _lines = vector<Line>
        {
            Line{0, 1, _points},
            Line{0, 2, _points},
            Line{1, 2, _points}
        };
    }

    bool Polygon::IsEnable() const
    {
        return _points.size() >= 3;
    }
    void Polygon::GetMinDisToOriginLine(Vector2& normal, float& depth) const
    {
        int index = GetMinDisToOriginLineIndex();
        if(index <  0)
            return;
        normal = _lines[index].normal;
        depth = _lines[index].distance;
    }
    bool Polygon::IsApproachBound(const Vector2& newPoint) const
    {
        int index = GetMinDisToOriginLineIndex();
        if(index <  0)
            return false;
        return Dot(newPoint, _lines[index].normal) - _lines[index].distance < Epsilon;
    }
    bool Polygon::AddPoint(const Vector2& newPoint)
    {
        _points.push_back(newPoint);
        UpdateVisibleLine(newPoint);
        auto boundPoints = GetVisibleLineBound();
        RemoveVisibleLine();
        if(boundPoints.size() < 2)
            return false;
        _lines.push_back(Line(boundPoints[0], static_cast<int>(_points.size() - 1), _points));
        _lines.push_back(Line(boundPoints[1], static_cast<int>(_points.size() - 1), _points));
        return true;
    }
    vector<Vector2> Polygon::GetPolygonPoints() const
    {
        unordered_set<int> pointSet;
        for(auto& line : _lines)
        {
            pointSet.insert(line.A);
            pointSet.insert(line.B);
        }
        vector<Vector2> ans;
        for(int i : pointSet)
            ans.push_back(_points[i]);
        return ans;
    }

    void Polygon::CompactLine()
    {
        partition(_lines.begin(), _lines.end(), [](const Line& l)->bool
        {
            return l.used;
        });
    }
    void Polygon::RemoveVisibleLine()
    {
        _lines.erase
        (
            remove_if(_lines.begin(), _lines.end(), [](const Line& l){ return !l.used; }),
            _lines.end()
        );
    }
    int Polygon::GetMinDisToOriginLineIndex() const
    {
        if(_lines.empty())
            return -1;
        float minDis = FLT_MAX;
        int minLine = -1;
        for(int i = 0; i < _lines.size(); i++)
        {
            if(_lines[i].distance < minDis)
            {
                minDis = _lines[i].distance;
                minLine = i;
            }
        }
        return minLine;
    }
    void Polygon::UpdateVisibleLine(const Vector2& newPoint)
    {
        for(auto& line : _lines)
            if(line.IsVisible(newPoint, _points))
                line.used = false;
    }
    vector<int> Polygon::GetVisibleLineBound()
    {
        CompactLine();
        unordered_map<int, int> pointMap;
        for(auto it = _lines.rbegin(); it != _lines.rend(); it++)
        {
            if(it->used)
                break;

            if(pointMap.count(it->A))
                pointMap[it->A]++;
            else
                pointMap.insert({it->A, 1});
            
            if(pointMap.count(it->B))
                pointMap[it->B]++;
            else
                pointMap.insert({it->B, 1});
        }

        vector<int> ans;
        for(auto& p : pointMap)
            if(p.second == 1)
                ans.push_back(p.first);
        return ans;
    }
}