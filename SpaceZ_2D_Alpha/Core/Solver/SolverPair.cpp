#include "Solver/SolverPair.h"
#include "Math/Math.h"
#include "Math/Vector3.h"
using namespace Core::Math;

namespace Core::Solver
{
    bool SolverPair::IsEnable() const
    {
        return first && second;
    }
    void SolverPair::SetRigidbody(SolverRigidbody* a, SolverRigidbody* b)
    {
        if(!a || !b)
            return;
        this->first = a;
        this->second = b;

        float crossA = Cross(point - a->position, normal);
        float crossB = Cross(point - b->position, normal);

        // 1 / effM = 1 / m + (r x n) ^ 2 / I
        float invEffMA = a->invMass + crossA * crossA * a->invInertia;
        float invEffMB = b->invMass + crossB * crossB * b->invInertia;

        invEffMass = invEffMA + invEffMB;
    }
    void SolverPair::SetContact(const Collider& a, const Collider& b)
    {
        staticFriction = Min(a.staticFriction, b.staticFriction);
        dynamicFriction = Min(a.dynamicFriction, b.dynamicFriction);

        elasticity = Min(a.elasticity, b.elasticity);
    }

    void SolverPair::ApplyNormalImpulse()
    {
        if(!first || !second)
            return;
        auto rA = point - first->position;
        auto rB = point - second->position;
        
        auto vA = first->GetRealVelocity(rA);
        auto vB = second->GetRealVelocity(rB);

        // first -> second
        auto nV = Dot(vA - vB, normal);
        nV = Max(0.0f, nV);

        normalImpulse = (1.0f + elasticity) / Max(invEffMass, Epsilon) * nV;
        auto p = normalImpulse * normal;
        first->ApplyImpulse(-p);
        second->ApplyImpulse(p);

        first->ApplyImpulseMoment(Cross(rA, -p));
        second->ApplyImpulseMoment(Cross(rB, p));
    }
    void SolverPair::ApplyTangentImpulse(float step)
    {
        if(!first || !second)
            return;
        auto rA = point - first->position;
        auto rB = point - second->position;

        auto v = first->GetRealVelocity(rA) - second->GetRealVelocity(rB);

        auto nP = normalImpulse * normal;
        auto nF = nP / step;

        // n x k
        auto t = Vector2(-normal.y, normal.x);

        auto tV = v.ProjectionVector(t);
        auto tP = 1.0f / Max(invEffMass, Epsilon) * tV;

        float fP = 0;
        if(tV.LengthSquared() < Epsilon * Epsilon)
        {
            // 是否在摩擦锥内
            if(tP.Length() <= staticFriction * nP.Length())
                fP = nF.Length() * staticFriction * step;
            else
                fP = nF.Length() * dynamicFriction * step;
        }
        else
            fP = nF.Length() * dynamicFriction * step;
        
        fP = Min(tP.Length(), fP);
        float sgn = (Dot(tV, t) >= 0.0f) ? 1.0f : -1.0f;
        tP -= sgn * fP * t;
        
        first->ApplyImpulse(-tP);
        second->ApplyImpulse(tP);

        first->ApplyImpulseMoment(Cross(rA, -tP));
        second->ApplyImpulseMoment(Cross(rB, tP));
    }
}