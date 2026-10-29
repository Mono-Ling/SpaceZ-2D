#include "Collision/Geometry/Polygon.h"
#include "Debug/Debug.h"
#include "Math/Math.h"
#include <algorithm>
#include <cfloat>
#include <unordered_map>
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    Line::Line(int a, int b, const vector<Vector2>& points) : used(true), A(a), B(b)
    {
        int len = static_cast<int>(points.size());
        if(a < 0 || b < 0 || a >= len || b >= len)
        {
            Debug::LogError("【多边形-边】顶点索引越界");
            return;
        }
        normal = (points[a] - points[b]).Normalized().Perp();
        if(Dot(normal, points[a]) < 0)
            normal = -normal;
        distance = PointToLineDistance(Vector2::zero, points[a], points[b]);
    }
    bool Line::IsVisible(const Vector2& point, const vector<Vector2>& points) const
    {
        return Dot(point - points[A], normal) > 0;
    }

    Polygon::Polygon(const array<Vector2, 3>& arr, int count)
    {
        if(count < 3)
            return;
        for(int i = 0; i < count; i++)
            points.push_back(arr[i]);
        lines.emplace_back(Line(0, 1, points));
        lines.emplace_back(Line(0, 2, points));
        lines.emplace_back(Line(1, 2, points));
    }
    Polygon::operator bool() const
    {
        return points.size() >= 3;
    }
    void Polygon::GetMinDisToOriginLine(Vector2& normal, float& depth) const
    {
        int line = GetMinDisToOriginLineIndex();
        if(line < 0)
        {
            Debug::LogError("【多边形】距原点最近边获取失败");
            return;
        }
        normal = lines[line].normal;
        depth = lines[line].distance;
    }
    bool Polygon::IsApproachBound(const Vector2& newPoint) const
    {
        int line = GetMinDisToOriginLineIndex();
        if(line < 0)
        {
            Debug::LogError("【多边形】距原点最近边获取失败");
            return true; // 退出迭代
        }
        return Dot(lines[line].normal, newPoint) - lines[line].distance < Epsilon;
    }
    void Polygon::AddPoint(const Vector2& newPoint)
    {
        points.push_back(newPoint);
        UpdateVisibleLine(newPoint);
        auto arr = GetVisibleBound();
        RemoveVisibleLine();
        for(auto i : arr)
            lines.emplace_back(Line(i, static_cast<int>(points.size() - 1), points));
    }

    void Polygon::RemoveVisibleLine()
    {
        lines.erase
        (
            remove_if(lines.begin(), lines.end(), [](const Line& line){ return !line.used; }),
            lines.end()
        );
    }
    void Polygon::UpdateVisibleLine(const Vector2& newPoint)
    {
        for(auto& line : lines)
            if(line.IsVisible(newPoint, points))
                line.used = false;
    }
    int Polygon::GetMinDisToOriginLineIndex() const
    {
        float minDis = FLT_MAX;
        int minLine = -1;
        for(int i = 0; i < lines.size(); i++)
            if(lines[i].distance < minDis)
            {
                minDis = lines[i].distance;
                minLine = i;
            }
        return minLine;
    }
    vector<int> Polygon::GetVisibleBound()
    {
        unordered_map<int, int> visibleMap; // index, count
        for(auto it = lines.rbegin(); it != lines.rend(); it++)
        {
            if(it->used)
                continue;
            visibleMap[it->A]++;
            visibleMap[it->B]++;
        }
        vector<int> ans;
        for(auto& p : visibleMap)
            if(p.second == 1)
                ans.emplace_back(p.first);
        return ans;
    }
}