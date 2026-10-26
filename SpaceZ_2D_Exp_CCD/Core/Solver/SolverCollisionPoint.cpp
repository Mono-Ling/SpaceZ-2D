#include "Solver/SolverCollisionPoint.h"
#include "Solver/PhyQuantities.h"
#include "Math/Math.h"
using namespace Core::Solver::PhyQuantities;
using namespace Core::Math;

namespace Core::Solver
{
    void SolverCollisionPoint::SetBody(SolverRigidbody* const first, SolverRigidbody* const second)
    {
        if(!first || !second)
            return;
        radiusVector = {point - first->position, point - second->position};

        invNormalEffMass = GetInvEffMass(normal, first, second);
        invTangentEffMass = GetInvEffMass(normal.Perp(), first, second);

        anchorPoint.first = first->PointWorldToLocal(anchorPoint.first);
        anchorPoint.second = second->PointWorldToLocal(anchorPoint.second);
    }
    void SolverCollisionPoint::SetTargetRelatVelocity(float elasticity, SolverRigidbody* const first, SolverRigidbody* const second)
    {
        if(!first || !second)
            return;
        auto vA = first->GetRealVelocity(radiusVector.first);
        auto vB = second->GetRealVelocity(radiusVector.second);
        float vN = Dot(vA - vB, normal);
        targetRelatVelocity = vN > Epsilon ? -elasticity * vN : 0.0f;
    }
    Vector2 SolverCollisionPoint::GetDeltaNormalImpulse(SolverRigidbody* const first, SolverRigidbody* const second) const
    {
        if(!first || !second)
            return Vector2::zero;
        auto vA = first->GetRealVelocity(radiusVector.first);
        auto vB = second->GetRealVelocity(radiusVector.second);

        float vN = Dot(vA - vB, normal);
        
        float pLen = (vN - targetRelatVelocity) / Max(invNormalEffMass, Epsilon);
        float total = Max(0.0f, pLen + normalImpulse);
        float delta = total - normalImpulse;
        normalImpulse = total;

        return delta * normal;
    }
    Vector2 SolverCollisionPoint::GetTangentImpulse(float sf, float df, SolverRigidbody* const first, SolverRigidbody* const second) const
    {
        if(!first || !second)
            return Vector2::zero;
        auto vA = first->GetRealVelocity(radiusVector.first);
        auto vB = second->GetRealVelocity(radiusVector.second);
        
        auto tangent = normal.Perp();
        float vT = Dot(vA - vB, tangent);
        float pT = vT / Max(invTangentEffMass, Epsilon);

        normalImpulse = Max(0.0f, normalImpulse);
        float fric = sf * normalImpulse;
        if(!Near(vT, 0.0f) && fric < Abs(pT))
            fric = df * normalImpulse;
        
        float total = Clamp(tangentImpulse + pT, -fric, fric);
        float delta = total - tangentImpulse;
        tangentImpulse = total;

        return delta * tangent;
    }
    Vector2 SolverCollisionPoint::GetPositionCorrection(SolverRigidbody* const first, SolverRigidbody* const second) const
    {
        if(!first || !second)
            return Vector2::zero;
        auto worldA = first->PointLocalToWorld(anchorPoint.first);
        auto worldB = second->PointLocalToWorld(anchorPoint.second);

        // depth为穿透深度，计算时应对碰撞体碰撞锚点顺序对调
        float depth = Dot(worldA - worldB, normal);
        depth = Clamp(depth - PENETRATE_SLOP, 0.0f, MAX_POS_CORRECTION);
        float corrLen = RELAXATION * depth / Max(invNormalEffMass, Epsilon);

        return corrLen * normal;
    }

    float SolverCollisionPoint::GetInvEffMass(const Vector2& dir, SolverRigidbody* const first, SolverRigidbody* const second) const
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