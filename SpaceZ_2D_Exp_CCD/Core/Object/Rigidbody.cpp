#include "Object/Rigidbody.h"
#include "Solver/PhyQuantities.h"
#include "Math/Math.h"
using namespace Core::Math;
using namespace Core::Solver::PhyQuantities;

namespace Core
{
    void Rigidbody::AddForce(const Vector2& f, ForceType type)
    {
        switch (type)
        {
        case ForceType::Impulse:
            ApplyImpulse(f);
            break;
        
        case ForceType::Sustain:
            force += f;
            break;
        }
    }
    void Rigidbody::AddTorque(float t, ForceType type)
    {
        switch (type)
        {
        case ForceType::Impulse:
            ApplyImpulseMoment(t);
            break;
        
        case ForceType::Sustain:
            torque += t;
            break;
        }
    }
    void Rigidbody::Step(float dt)
    {
        if(dt <= 0)
            return;

        if(useGravity)
            AddForce(Vector2::down * mass * GRAVITY_ACCELERATION, ForceType::Sustain);

        ApplyImpulse(force * dt);
        ApplyImpulseMoment(torque * dt);
        force = Vector2::zero;
        torque = 0.0f;
    }
    void Rigidbody::StepTransform(float dt)
    {
        if(dt <= 0)
            return;
        linearVelocity *= Exp(-linearDrag * dt);
        angularVelocity *= Exp(-angularDrag * dt);
        
        transform.Position(transform.Position() + linearVelocity * dt);
        transform.Rotation(transform.Rotation() + angularVelocity * dt);
    }

    void Rigidbody::ApplyImpulse(const Vector2& impulse)
    {
        linearVelocity += impulse / Max(mass, Epsilon);
    }
    void Rigidbody::ApplyImpulseMoment(float moment)
    {
        angularVelocity += moment / Max(inertia, Epsilon);
    }
}