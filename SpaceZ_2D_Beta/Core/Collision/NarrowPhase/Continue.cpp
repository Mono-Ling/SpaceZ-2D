#include "Core/Collision/NarrowPhase/NarrowPhase.h"
#include "Core/Math/Math.h"
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    Vector2 GetOffsetDir(const ConvexDetectReq& first, const ConvexDetectReq& second)
    {
        if(!first || !second)
            return Vector2::zero;
        Vector2 dir = (second.Position() - first.Position()).Normalized();
        if(dir == Vector2::zero)
            dir = Vector2::up;
        return dir;
    }

    bool ContinueCollision(ContinueDetectReq& req, float& time, vector<CollisionInfo>& infos)
    {
        infos.clear();
        if(!req || time < 0.0f)
            return false;

        float endTime = time;
        time = 0.0f;
        Simplex simplex;
        int iteration = 0;
        bool isCollision = GJK(req.first, req.second, simplex);
        while(time < endTime && !isCollision)
        {
            float far = simplex.SeparateDis();

            if(far < 0)
            {
                auto dir = GetOffsetDir(req.first, req.second);
                far = Abs(Dot(MinkowskiDiff(dir, req.first, req.second), dir));
            }

            float vA = req.linearVelocity.first.Length()
                     + Distance(req.first.GuardMaxDistancePoint(), req.first.Position())
                     * Abs(req.angularVelocity.first);
            float vB = req.linearVelocity.second.Length()
                     + Distance(req.second.GuardMaxDistancePoint(), req.second.Position())
                     * Abs(req.angularVelocity.second);
            float v = vA + vB;
            if(LessNear(v, 0.0f))
                return false;

            float dt = Max(far, Epsilon) / v;
            dt = Clamp(dt, Epsilon, endTime - time);
            time += dt;
            req.Step(dt);

            simplex.count = 0;
            isCollision = GJK(req.first, req.second, simplex);

            if(++iteration >= MAX_ITERATION)
            {
                if(!isCollision)
                {
                    req.Step(endTime - time);
                    time = endTime;
                }
                break;
            }
        }
        time = Clamp(time, 0.0f, endTime);
        if(!isCollision)
            return false;
        auto info = EPA(req.first, req.second, simplex.points);
        if(info.depth < 0 || info.normal == Vector2::zero)
            return false;
        infos = ContactManifold(req.first, req.second, info);
        return true;
    }
}