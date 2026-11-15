#include "Core/Collision/Geometry/Polygon.h"
#include "Core/Debug/DebugFormat.h"
#include <cfloat>
#include <algorithm>
#include <unordered_map>
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    Polygon::Line::Line(int a, int b, const vector<Vector2>& points)
    {
        if(a < 0 || b < 0 || a >= points.size() || b >= points.size())
        {
            Debug::LogErrorFormat("【Polygon-Line】无效顶点索引", a, "|", b);
            used = false;
            return;
        }
        A = a;
        B = b;
        if(points[A] == points[B])
        {
            used = false;
            return;
        }

        used = true;
        normal = (points[B] - points[A]).Normalized().Perp();
        if(Dot(points[A], normal) < 0)
            normal = -normal;
        distance = PointToLineSegDistance(Vector2::zero, points[A], points[B]);
    }
    bool Polygon::Line::Visible(const Math::Vector2& newPoint, const vector<Vector2>& points)
    {
        return Dot(newPoint - points[A], normal) > 0;
    }

    Polygon::Polygon(const array<Vector2, 3>& arr)
    {
        _points.emplace_back(arr[0]);
        _points.emplace_back(arr[1]);
        _points.emplace_back(arr[2]);

        _lines.emplace_back(0, 1, _points);
        _lines.emplace_back(0, 2, _points);
        _lines.emplace_back(1, 2, _points);

        for(auto& line : _lines)
        {
            if(line.used)
                continue;
            Debug::LogError("【Polygon】边初始化失败");
            _points.clear();
            _lines.clear();
            break;
        }
    }

    Polygon::operator bool() const
    {
        return _points.size() >= 3;
    }
    void Polygon::MinLineToOriginDis(Vector2& normal, float& depth) const
    {
        int index = GetMinLineToOriginDisIndex();
        if(index < 0)
        {
            Debug::LogError("【Polygon】到原点距离最短边获取失败");
            return;
        }
        normal = _lines[index].normal;
        depth = _lines[index].distance;
    }
    bool Polygon::IsApproachBound(const Vector2& point) const
    {
        int index = GetMinLineToOriginDisIndex();
        if(index < 0)
        {
            Debug::LogError("【Polygon】到原点距离最短边获取失败");
            return false;
        }
        return Dot(_lines[index].normal, point) - _lines[index].distance < Epsilon;
    }
    void Polygon::AddPoint(const Vector2& point)
    {
        _points.emplace_back(point);
        SignVisiblePoint(point);
        auto boundPoints = VisibleBound();
        for(auto& i : boundPoints)
            if(_points[i] != point)
                _lines.emplace_back(i, _points.size() - 1, _points);
        RemoveVisibleLine();
    }

    int Polygon::GetMinLineToOriginDisIndex() const
    {
        int ans = -1;
        float minDis = FLT_MAX;
        for(int i = 0; i < _lines.size(); i++)
            if(_lines[i].distance < minDis)
            {
                ans = i;
                minDis = _lines[i].distance;
            }
        return ans;
    }
    void Polygon::SignVisiblePoint(const Vector2& newPoint)
    {
        for(auto& line : _lines)
            if(line.Visible(newPoint, _points))
                line.used = false;
    }
    vector<int> Polygon::VisibleBound()
    {
        _visiblePoints.clear();
        for(auto& line : _lines)
            if(!line.used)
            {
                _visiblePoints[line.A]++;
                _visiblePoints[line.B]++;
            }

        vector<int> ans;
        for(auto& p : _visiblePoints)
            if(p.second == 1)
                ans.emplace_back(p.first);
        return ans;
    }
    void Polygon::RemoveVisibleLine()
    {
        _lines.erase
        (
            remove_if(_lines.begin(), _lines.end(), [&](const Line& line){ return !line.used; }),
            _lines.end()
        );
    }
}