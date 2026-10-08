#include "Solver/SolverRigidbody.h"
using namespace Core::Math;

namespace Core::Solver
{
    void SolverRigidbody::Reset()
    {
        invMass = 0;
        invInertia = 0;
    }
    void SolverRigidbody::Reset(float m, float i)
    {
        invMass = 1 / m;
        invInertia = 1 / i;
    }
    void SolverRigidbody::ApplyImpulse(const Vector2& p)
    {
        linearVelocity += p * invMass;
    }
    void SolverRigidbody::ApplyImpulseMoment(float momentImpulse)
    {
        angularVelocity += momentImpulse * invInertia;
    }

    Vector2 SolverRigidbody::GetRealVelocity(const Math::Vector2& r) const
    {
        // totalV = v + angular x r
        return linearVelocity + Vector2(-angularVelocity * r.y, angularVelocity * r.x);
    }
}
