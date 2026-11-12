#include "Core/Solver/SolverRigidbody.h"
#include "Core/Math/Matrix3x3.h"
#include "Core/Math/Math.h"
using namespace Core::Math;

namespace Core::Solver
{
    void SolverRigidbody::SetBody()
    {
        invMass = 0.0f;
        invInertia = 0.0f;
    }
    void SolverRigidbody::SetBody(float m, float i)
    {
        invMass = 1.0f / Max(m, Epsilon);
        invInertia = 1.0f / Max(i, Epsilon);
    }
    Vector2 SolverRigidbody::RealVelocity(const Vector2& r) const
    {
        return linearVelocity + Vector2(-angularVelocity * r.y, angularVelocity * r.x);
    }

    void SolverRigidbody::ApplyImpulse(const Vector2& impulse)
    {
        linearVelocity += impulse * invMass;
    }
    void SolverRigidbody::ApplyImpulseMoment(float moment)
    {
        angularVelocity += moment * invInertia;
    }

    void SolverRigidbody::ApplyPositionImpulse(const Vector2& p)
    {
        position += p * invMass;
    }
    void SolverRigidbody::ApplyAngleImpulse(float m)
    {
        rotation += m * invInertia;
    }

    Vector2 SolverRigidbody::PointLocalToWorld(const Vector2& point) const
    {
        auto m = TR(position, rotation);
        return MulPoint(m, point);
    }
    Vector2 SolverRigidbody::PointWorldToLocal(const Vector2& point) const
    {
        auto m = InverseTR(position, rotation);
        return MulPoint(m, point);
    }
}