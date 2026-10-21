#include "CollisionDetection/NarrowPhase/NarrowPhase.h"
#include "CollisionDetection/NarrowPhase/ConvexDetectReq.h"
#include "CollisionDetection/Geometry/Polygon.h"
#include "Math/Vector2.h"
#include "Math/Math.h"
#include <algorithm>
using namespace std;
using namespace Core::Math;

namespace Core::Collision
{
    void NarrowPhase::ClearCollisionPairs()
    {
        _pairs.clear();
    }
    void NarrowPhase::SetDetectPairs(vector<NarrowPhaseDetectPair>& pairs)
    {
        ClearCollisionPairs();
        if(pairs.empty())
            return;
        for(auto& p : pairs)
        {
            p.isCollision = false;
            CollisionDetection(p);
            
            ContactManifold(p, _pairs);
        }
    }
    const vector<CollisionPair>& NarrowPhase::GetCollisionPairs() const
    {
        return _pairs;
    }

    void NarrowPhase::CollisionDetection(NarrowPhaseDetectPair& p)
    {
        if(!p.first.IsEnable() || !p.second.IsEnable())
            return;
        Simplex simplex;
        if(!GJK(p.first, p.second, simplex))
            return;
        
        p.isCollision = true;
        if(CompleteSimplex(p.first, p.second, simplex))
            EPA(p.first, p.second, simplex, p.collisionPair);
    }
    bool NarrowPhase::CompleteSimplex(const ConvexDetectReq& first, const ConvexDetectReq& second, Simplex& simplex)
    {
        if(!first.IsEnable() || !second.IsEnable())
            return false;
        Vector2 update;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            if(simplex.TryComplete(update))
                return true;
            auto point = GetMinkowskiDiff(update, first, second);
            simplex.AddPoint(point);
        }
        return false;
    }
    bool NarrowPhase::GJK(const ConvexDetectReq& first, const ConvexDetectReq& second, Simplex& simplex)
    {
        if(!first.IsEnable() || !second.IsEnable())
            return false;
        Vector2 update = second.WorldPosition() - first.WorldPosition();
        update += (update == Vector2::zero) * Vector2::up;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            auto point = GetMinkowskiDiff(update, first, second);
            if(Dot(point, update) < Epsilon)
                return false;
            simplex.AddPoint(point);
            if(simplex.ContainsOrigin(update))
                return true;
        }
        return false;
    }
    void NarrowPhase::EPA(const ConvexDetectReq& first, const ConvexDetectReq& second,
                          const Simplex& simplex, CollisionPair& p)
    {
        Polygon polygon(simplex);
        if(!polygon.IsEnable())
            return;
        float depth;
        Vector2 normal;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            polygon.GetMinDisToOriginLine(normal, depth);
            auto newPoint = GetMinkowskiDiff(normal, first, second);
            if(polygon.IsApproachBound(newPoint))
                break;
            polygon.AddPoint(newPoint);
        }
        p.depth = depth;
        p.normal = normal;
        auto wFirst = first.SurfacePoint(normal);
        auto wSecond = second.SurfacePoint(-normal);
        p.anchorPoint = {first.PointWorldToLocalRT(wFirst), second.PointWorldToLocalRT(wSecond)};
        p.point = (wFirst + wSecond) / 2.0f;
    }
    void NarrowPhase::ContactManifold(NarrowPhaseDetectPair& p, vector<CollisionPair>& ans)
    {
        if(!p.isCollision)
            return;
        auto point = p.collisionPair.point;
        auto normal = p.collisionPair.normal.Normalized();
        auto tangent = normal.Perp();

        auto lineA = p.first.GetEdge(-normal);
        auto lineB = p.second.GetEdge(normal);

        pair<Vector2, Vector2> lineAT
        {
            lineA.first.ProjectionPoint(point, tangent),
            lineA.second.ProjectionPoint(point, tangent)
        };
        pair<Vector2, Vector2> lineBT
        {
            lineB.first.ProjectionPoint(point, tangent),
            lineB.second.ProjectionPoint(point, tangent)
        };

        pair<float, float> lineAOffset
        {
            Dot(lineAT.first - point, tangent),
            Dot(lineAT.second - point, tangent)
        };
        pair<float, float> lineBOffset
        {
            Dot(lineBT.first - point, tangent),
            Dot(lineBT.second - point, tangent)
        };

        if(lineAOffset.first > lineAOffset.second)
            swap(lineAOffset.first, lineAOffset.second);
        if(lineBOffset.first > lineBOffset.second)
            swap(lineBOffset.first, lineBOffset.second);

        if(lineAOffset.first > lineBOffset.second
        || lineBOffset.first > lineAOffset.second)
        {
            ans.push_back(p.collisionPair);
            return;
        }

        pair<Vector2, Vector2> line
        {
            tangent * Max(lineAOffset.first, lineBOffset.first) + point,
            tangent * Min(lineAOffset.second, lineBOffset.second) + point
        };

        p.collisionPair.point = line.first;
        p.collisionPair.anchorPoint = 
        {
            p.first.PointWorldToLocalRT(line.first.ProjectionPoint(lineA.first, (lineA.first - lineA.second))),
            p.second.PointWorldToLocalRT(line.first.ProjectionPoint(lineB.first, (lineB.first - lineB.second)))
        };
        ans.push_back(p.collisionPair);

        if(line.first == line.second)
            return;

        p.collisionPair.point = line.second;
        p.collisionPair.anchorPoint = 
        {
            p.first.PointWorldToLocalRT(line.second.ProjectionPoint(lineA.first, (lineA.first - lineA.second))),
            p.second.PointWorldToLocalRT(line.second.ProjectionPoint(lineB.first, (lineB.first - lineB.second)))
        };
        ans.push_back(p.collisionPair);
    }
}