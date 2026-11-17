#include "Core/Collision/NarrowPhase/NarrowPhase.h"
#include "Core/Collision/Geometry/Polygon.h"
#include "Core/Math/Math.h"
#include <utility>
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    bool GJK(const ConvexDetectReq& first, const ConvexDetectReq& second, Simplex& simplex, Vector2* separateAxis)
    {
        if(separateAxis)
            *separateAxis = Vector2::zero;

        if(!first || !second)
            return false;
        Vector2 dir = second.Position() - first.Position();
        if(dir == Vector2::zero)
            dir = Vector2::up;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            auto point = MinkowskiDiff(dir, first, second);
            if(LessNear(Dot(point, dir), 0.0f))
            {
                if(separateAxis)
                *separateAxis = -dir;
                return false;
            }
            if(!simplex.AddPoint(point))
            {
                dir = dir.Perp();
                continue;
            }
            if(simplex.ContainsOrigin(dir) && simplex.TryComplete(dir))
                return true;
        }
        if(separateAxis)
            *separateAxis = -dir;
        return false;
    }
    CollisionInfo EPA(const ConvexDetectReq& first, const ConvexDetectReq& second, const array<Vector2, 3>& points)
    {
        if(!first || !second)
            return {};
        Polygon polygon(points);
        if(!polygon)
            return {};
        Vector2 normal;
        float depth = 0.0f;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            polygon.MinLineToOriginDis(normal, depth);
            auto point = MinkowskiDiff(normal, first, second);
            if(polygon.IsApproachBound(point))
                break;
            polygon.AddPoint(point);
        }
        CollisionInfo info;
        info.depth = depth;
        info.normal = normal.Normalized();
        info.point = (first.Support(normal) + second.Support(-normal)) / 2.0f;
        return info;
    }
    vector<CollisionInfo> ContactManifold(const ConvexDetectReq& first, const ConvexDetectReq& second, const CollisionInfo& info)
    {
        vector<CollisionInfo> ans;
        if(!first || !second)
            return ans;
        if(info.depth < 0.0f || info.normal == Vector2::zero)
            return ans;

        auto point = info.point;
        auto normal = info.normal.Normalized();
        auto tangent = normal.Perp();

        auto lineA = first.Edge(-normal);
        auto lineB = second.Edge(normal);

        pair<float, float> offsetA
        {
            Dot(lineA.first - point, tangent),
            Dot(lineA.second - point, tangent),
        };
        pair<float, float> offsetB
        {
            Dot(lineB.first - point, tangent),
            Dot(lineB.second - point, tangent),
        };

        if(offsetA.first > offsetA.second)
            swap(offsetA.first, offsetA.second);
        if(offsetB.first > offsetB.second)
            swap(offsetB.first, offsetB.second);
        
        if(offsetA.first > offsetB.second || offsetB.first > offsetA.second)
        {
            ans.emplace_back(info);
            return ans;
        }

        pair<float, float> line
        {
            Max(offsetA.first, offsetB.first),
            Min(offsetA.second, offsetB.second),
        };

        CollisionInfo output;
        output.normal = normal;
        output.depth = info.depth;
        output.point = point + tangent * line.first;
        ans.push_back(output);

        if(Near(line.first, line.second))
            return ans;
        
        output.point = point + tangent * line.second;
        ans.push_back(output);
        return ans;
    }
}