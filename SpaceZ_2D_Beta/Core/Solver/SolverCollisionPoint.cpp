#include "Core/Solver/SolverCollisionPoint.h"
#include "Core/Solver/PhyQuantities.h"
#include "Core/Math/Math.h"
using namespace Core::Solver::PhyQuantities;
using namespace Core::Collision;
using namespace Core::Math;
using namespace std;

namespace Core::Solver
{
    SolverCollisionPoint::SolverCollisionPoint(const CollisionInfo& info, const SolverRigidbody* first, const SolverRigidbody* second)
    : point(info.point), normal(info.normal.Normalized())
    {
        if(!first || !second)
            return;
        radiusVector = 
        {
            point - first->position,
            point - second->position,
        };

        float halfDepth = info.depth / 2.0f;
        anchorPoint = 
        {
            first->PointWorldToLocal(point + normal * halfDepth),
            second->PointWorldToLocal(point - normal * halfDepth),
        };
        
        invNormalEffMass = GetInvEffMass(normal, first, second);
        invTangentEffMass = GetInvEffMass(normal.Perp(), first, second);
    }

    void SolverCollisionPoint::SetTargetRelatVelocity(float elasticity, const SolverRigidbody* first, const SolverRigidbody* second)
    {
        if(!first || !second)
            return;
        auto vN = first->RealVelocity(radiusVector.first) - second->RealVelocity(radiusVector.second);
        float vNLen = Dot(vN, normal);
        targetRelatVelocity = vNLen > Epsilon ? -elasticity * vNLen : 0.0f;
    }
    Vector2 SolverCollisionPoint::GetDeltaNormalImpulse(const SolverRigidbody* first, const SolverRigidbody* second) const
    {
        if(!first || !second)
            return Vector2::zero;
        auto vN = first->RealVelocity(radiusVector.first) - second->RealVelocity(radiusVector.second);
        float vNLen = Dot(vN, normal);
        
        float deltaI = (vNLen - targetRelatVelocity) / Max(invNormalEffMass, Epsilon);
        float totalI = Max(normalImpulse + deltaI, 0.0f);

        deltaI = totalI - normalImpulse;
        normalImpulse = totalI;

        return deltaI * normal;
    }
    Vector2 SolverCollisionPoint::GetDeltaTangentImpulse(float sf, float df, const SolverRigidbody* first, const SolverRigidbody* second) const
    {
        if(!first || !second)
            return Vector2::zero;
        auto tangent = normal.Perp();
        auto vT = first->RealVelocity(radiusVector.first) - second->RealVelocity(radiusVector.second);
        float vTLen = Dot(vT, tangent);

        float deltaI = vTLen / Max(invTangentEffMass, Epsilon);

        float fI = normalImpulse * sf;
        if(fI < Abs(deltaI + tangentImpulse))
            fI = df * normalImpulse;
        
        float totalI = Clamp(deltaI + tangentImpulse, -fI, fI);
        deltaI = totalI - tangentImpulse;
        tangentImpulse = totalI;

        return deltaI * tangent;
    }
    Vector2 SolverCollisionPoint::GetPositionCorrection(const SolverRigidbody* first, const SolverRigidbody* second) const
    {
        if(!first || !second)
            return Vector2::zero;
        auto offset = first->PointLocalToWorld(anchorPoint.first) - second->PointLocalToWorld(anchorPoint.second);
        float depth = Dot(offset, normal);

        if(LessNear(depth, 0.0f, PENETRATE_SLOP))
            return Vector2::zero;

        depth = Clamp(depth, 0.0f, MAX_POS_CORRECTION);
        float corr = RELAXATION * depth / Max(invNormalEffMass, Epsilon);

        return corr * normal;
    }

    float SolverCollisionPoint::GetInvEffMass(const Vector2& dir, const SolverRigidbody* first, const SolverRigidbody* second) const
    {
        if(!first || !second)
            return 0.0f;
        auto d = dir.Normalized();
        float crossA = Cross(radiusVector.first, d);
        float crossB = Cross(radiusVector.second, d);

        // 1 / effM = 1 / m + (r x n) ^ 2 / I
        float invEffMA = first->invMass + crossA * crossA * first->invInertia;
        float invEffMB = second->invMass + crossB * crossB * second->invInertia;
        return invEffMA + invEffMB;
    }
}