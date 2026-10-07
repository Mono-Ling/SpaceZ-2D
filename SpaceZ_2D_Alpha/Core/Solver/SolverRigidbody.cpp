#include "Solver/SolverRigidbody.h"
using namespace Core::Math;

namespace Core::Solver
{
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
