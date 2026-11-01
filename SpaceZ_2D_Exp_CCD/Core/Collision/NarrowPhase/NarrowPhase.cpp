#include "Collision/NarrowPhase/NarrowPhase.h"
#include "Math/Math.h"
#include "Debug/Debug.h"
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    bool GJK(const ConvexDetectReq& first, const ConvexDetectReq& second, Simplex& simplex)
    {
        if(!first || !second)
            return false;
        simplex.count = 0;
        Vector2 update = second.Position() - first.Position();
        update += (update == Vector2::zero) * Vector2::up;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            auto point = MinkowskiDiff(update, first, second);
            if(Dot(point, update) < 0)
                return false;
            if(!simplex.AddPoint(point))
            {
                update = update.Perp();
                continue;
            }
            if(simplex.ContainsOrigin(update))
                return true;
        }
        return false;
    }
    CollisionInfo EPA(const ConvexDetectReq& first, const ConvexDetectReq& second, const array<Vector2, 3>& points)
    {
        if(!first || !second)
            return {};
        Polygon polygon(points, 3);
        if(!polygon)
            return {};
        Vector2 normal;
        float depth = 0.0f;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            polygon.GetMinDisToOriginLine(normal, depth);
            auto point = MinkowskiDiff(normal, first, second);
            if(polygon.IsApproachBound(point))
                break;
            polygon.AddPoint(point);
        }
        CollisionInfo ans;
        ans.point = (first.Support(normal) + second.Support(-normal)) / 2.0f;
        ans.normal = normal.Normalized();
        ans.depth = depth;
        return ans;
    }
    vector<CollisionInfo> ContactManifold(const ConvexDetectReq& first, const ConvexDetectReq& second, const CollisionInfo& info)
    {
        vector<CollisionInfo> ans;
        if(!first || !second)
            return ans;
        auto point = info.point;
        auto normal = info.normal.Normalized();
        auto tangent = normal.Perp();

        auto lineA = first.Edge(-normal);
        auto lineB = second.Edge(normal);

        pair<float, float> lineAOffset
        {
            Dot(lineA.first - point, tangent),
            Dot(lineA.second - point, tangent)
        };
        pair<float, float> lineBOffset
        {
            Dot(lineB.first - point, tangent),
            Dot(lineB.second - point, tangent)
        };

        if(lineAOffset.first > lineAOffset.second)
            swap(lineAOffset.first, lineAOffset.second);
        if(lineBOffset.first > lineBOffset.second)
            swap(lineBOffset.first, lineBOffset.second);

        if(lineAOffset.first > lineBOffset.second
        || lineBOffset.first > lineAOffset.second)
        {
            ans.push_back(info);
            return ans;
        }

        pair<Vector2, Vector2> line
        {
            tangent * Max(lineAOffset.first, lineBOffset.first) + point,
            tangent * Min(lineAOffset.second, lineBOffset.second) + point,
        };

        ans.emplace_back(CollisionInfo(info.depth, normal, line.first));

        if(line.first == line.second)
            return ans;
        
        ans.emplace_back(CollisionInfo(info.depth, normal, line.second));
        return ans;
    }
}