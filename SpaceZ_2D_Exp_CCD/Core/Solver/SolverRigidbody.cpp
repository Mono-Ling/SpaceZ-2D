#include "Solver/SolverRigidbody.h"
#include "Math/Matrix3x3.h"
#include "Math/Math.h"
using namespace Core::Math;

namespace Core::Solver
{
    void SolverRigidbody::SetBody()
    {
        invMass = 0.0f;
        invInertia = 0.0f;
    }
    void SolverRigidbody::SetBody(float mass, float inertia)
    {
        invMass = 1.0f / Max(mass, Epsilon);
        invInertia = 1.0f/ Max(inertia, Epsilon);
    }
    Vector2 SolverRigidbody::GetRealVelocity(const Vector2& r) const
    {
        // totalV = v + angular x r
        return linearVelocity + Vector2(-angularVelocity * r.y, angularVelocity * r.x);
    }
    void SolverRigidbody::ApplyImpulse(const Vector2& p)
    {
        linearVelocity += p * invMass;
    }
    void SolverRigidbody::ApplyImpulseMoment(float m)
    {
        angularVelocity += m * invInertia;
    }

    void SolverRigidbody::ApplyPositionImpulse(const Vector2& p)
    {
        position += p * invMass;
    }
    void SolverRigidbody::ApplyAngleImpulse(float m)
    {
        angle += m * invInertia;
    }

    Vector2 SolverRigidbody::PointLocalToWorld(const Vector2& point) const
    {
        auto m = TR(position, angle);
        return MulPoint(m, point);
    }
    Vector2 SolverRigidbody::PointWorldToLocal(const Vector2& point) const
    {
        auto m = InverseTR(position, angle);
        return MulPoint(m, point);
    }
}