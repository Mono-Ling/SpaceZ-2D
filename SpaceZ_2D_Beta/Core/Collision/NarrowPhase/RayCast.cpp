#include "Core/Collision/NarrowPhase/NarrowPhase.h"
#include "Core/Math/Math.h"
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    bool LineGJK(const Vector2& start, const Vector2& end, const ConvexDetectReq& convex)
    {
        if(!convex)
            return false;
        Simplex simplex;
        Vector2 dir = (convex.Position() - (start + end) / 2.0f).Normalized();
        if(dir == Vector2::zero)
            dir = Vector2::up;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            auto point = convex.Support(-dir);
            point = Dot(dir, start) > Dot(dir, end) ? start - point : end - point;

            if(Dot(point, dir) < 0.0f)
                return false;
            if(!simplex.AddPoint(point))
            {
                dir = dir.Perp();
                continue;
            }
            if(simplex.ContainsOrigin(dir))
                return true;
        }
        return false;
    }
    bool PointGJK(const Vector2& point, const ConvexDetectReq& convex, Simplex& simplex, Vector2* separateAxis = nullptr)
    {
        if(separateAxis)
            *separateAxis = Vector2::zero;
        if(!convex)
            return false;
        Vector2 dir = (convex.Position() - point).Normalized();
        if(dir == Vector2::zero)
            dir = Vector2::up;
        for(int i = 0; i < MAX_ITERATION; i++)
        {
            auto newPoint = point - convex.Support(-dir);
            if(Dot(newPoint, dir) < 0.0f)
            {
                if(separateAxis)
                    *separateAxis = dir;
                return false;
            }
            if(!simplex.AddPoint(newPoint))
            {
                dir = dir.Perp();
                continue;
            }
            if(simplex.ContainsOrigin(dir))
            {
                if(separateAxis)
                    *separateAxis = dir;
                return true;
            }
        }
        if(separateAxis)
            *separateAxis = dir;
        return false;
    }

    bool RayCast(const Ray& ray, const ConvexDetectReq& convex, RayHit& rayHit)
    {
        rayHit = {};
        if(!ray || !convex)
            return false;
        if(!LineGJK(ray.origin, ray.origin + ray.direction * ray.maxDis, convex))
            return false;

        Simplex simplex;
        Vector2 point = ray.origin;
        Vector2 separateAxis;
        float distance = 0.0f;
        bool isIntersect = PointGJK(point, convex, simplex, &separateAxis);
        int count = 0;

        while (!isIntersect && distance < ray.maxDis && count++ < RAY_CAST_MAX_ITERATION)
        {
            separateAxis = separateAxis.Normalized();
            float far = simplex.SeparateDis();
            if(!simplex.count)
            {
                separateAxis = (convex.Position() - point).Normalized();
                if(separateAxis == Vector2::zero)
                    separateAxis = Vector2::up;
                far = Dot(convex.Support(-separateAxis) - point, separateAxis);
            }
            
            float proj = Dot(separateAxis, ray.direction);
            if(LessNear(proj, 0.0f))
                return false;

            float dx = far / proj;
            if(dx + distance > ray.maxDis)
                return false;

            dx = Max(dx, Epsilon);
            distance += dx;
            point += ray.direction * dx;

            simplex.count = 0;
            isIntersect = PointGJK(point, convex, simplex, &separateAxis);
        }
        if(!isIntersect)
            return false;
        rayHit.distance = distance;
        rayHit.point = point;

        auto normal = simplex.MinDisEdgeNormal().Normalized();
        rayHit.normal = Dot(normal, ray.direction) < 0.0f ? normal : -normal;
        return true;
    }
}