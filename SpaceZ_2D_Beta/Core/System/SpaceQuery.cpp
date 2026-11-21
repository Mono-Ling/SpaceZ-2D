#include "Core/System/PhySystem.h"
#include "Core/Collision/NarrowPhase/NarrowPhase.h"
#include "Core/Collision/Convex/Box.h"
#include "Core/Collision/Convex/Sphere.h"
#include "Core/Collision/Convex/Capsule.h"
#include "Core/Collision/Geometry/Ray.h"
#include <cfloat>
#include <algorithm>
using namespace Core::Collision;
using namespace Core::Math;
using namespace std;

namespace Core
{
    vector<ColliderHandle> PhySystem::OverlapBox(const Vector2& center, const Vector2& size, float rotation, const LayerMask& layer) const
    {
        if(LessNear(size.x, 0.0f) || LessNear(size.y, 0.0f))
            return {};
        Box box;
        box.SetBox(size / 2.0f);
        return OverlapConvex(box, center, rotation, layer);
    }
    vector<ColliderHandle> PhySystem::OverlapSphere(const Vector2& center, float radius, float rotation, const LayerMask& layer) const
    {
        if(LessNear(radius, 0.0f))
            return {};
        Sphere sphere;
        sphere.SetSphere(radius);
        return OverlapConvex(sphere, center, rotation, layer);
    }
    vector<ColliderHandle> PhySystem::OverlapCapsule(const Vector2& center, float radius, float height, float rotation, const LayerMask& layer) const
    {
        if(LessNear(radius, 0.0f) || LessNear(height, 0.0f))
            return {};
        Capsule capsule;
        capsule.SetCapsule(radius, height);
        return OverlapConvex(capsule, center, rotation, layer);
    }
    bool PhySystem::Raycast(const Vector2& origin, const Vector2& direction, RaycastHit& hit, float maxDis, const LayerMask& layer) const
    {
        hit = {};
        if(direction == Vector2::zero)
            return false;
        Ray ray(origin, direction, maxDis);
        auto func = [&](const LayerMask& objLayer, const Bound& objBound)->bool
        {
            return IsIntersect(layer, objLayer) && ray.IsIntersect(objBound);
        };
        auto pairs = _broadPhase.Overlap(func);

        float minDis = FLT_MAX;
        RaycastHit minHit;
        for(auto& p : pairs)
        {
            auto req = CreateConvexDetectReq(p.first);
            if(!req)
                continue;

            RayHit currHit;
            if(!RayCast(ray, req, currHit) || !currHit)
                continue;
            if(currHit.distance < minDis)
            {
                minDis = currHit.distance;
                minHit = RaycastHit(p.first, currHit);
            }
        }
        if(Near(minDis, FLT_MAX))
            return false;
        hit = minHit;
        return true;
    }
    vector<RaycastHit> PhySystem::RaycastAll(const Vector2& origin, const Vector2& direction, float maxDis, const LayerMask& layer) const
    {
        vector<RaycastHit> ans;
        if(direction == Vector2::zero)
            return ans;

        Ray ray(origin, direction, maxDis);
        auto func = [&](const LayerMask& objLayer, const Bound& objBound)->bool
        {
            return IsIntersect(layer, objLayer) && ray.IsIntersect(objBound);
        };
        auto pairs = _broadPhase.Overlap(func);

        for(auto& p : pairs)
        {
            auto req = CreateConvexDetectReq(p.first);
            if(!req)
                continue;

            RayHit hit;
            if(!RayCast(ray, req, hit) || !hit)
                continue;
            ans.emplace_back(p.first, hit);
        }
        sort(ans.begin(), ans.end(), [](const auto& a, const auto& b)
        {
            return a.distance < b.distance;
        });
        return ans;
    }

    vector<ColliderHandle> PhySystem::OverlapConvex(const Convex& convex, const Vector2& center, float rotation, const LayerMask& layer) const
    {
        vector<ColliderHandle> ans;

        auto local2world = TR(center, rotation);
        auto world2local = InverseTR(center, rotation);
        ConvexDetectReq req(&convex, local2world, world2local);

        Vector2 min
        {
            req.Support(Vector2::left).x,
            req.Support(Vector2::down).y,
        };
        Vector2 max
        {
            req.Support(Vector2::right).x,
            req.Support(Vector2::up).y,
        };
        auto bound = Bound::From(min, max);

        auto pairs = _broadPhase.BoundOverlap(bound, layer);
        Simplex simplex;
        for(auto& p : pairs)
        {
            auto collider = CreateConvexDetectReq(p.first);
            if(!collider)
                continue;
            
            if(GJK(collider, req, simplex))
                ans.emplace_back(p.first);
            simplex.count = 0;
        }
        return ans;
    }
}