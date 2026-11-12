#include "Core/Object/PhyObject/Rigidbody.h"
#include "Core/Solver/PhyQuantities.h"
#include "Core/Math/Math.h"
using namespace Core::Math;
using namespace Core::Solver;
using namespace std;

namespace Core
{
    void Rigidbody::AddForce(const Vector2& force, ForceMode mode)
    {
        switch(mode)
        {
        case ForceMode::Impulse:
            ApplyImpulse(force);
        break;

        case ForceMode::Sustain:
            this->force += force;
        break;
        }
    }
    void Rigidbody::AddTorque(float torque, ForceMode mode)
    {
        switch(mode)
        {
        case ForceMode::Impulse:
            ApplyImpulseMoment(torque);
        break;

        case ForceMode::Sustain:
            this->torque += torque;
        break;
        }
    }
    void Rigidbody::Step(float dt)
    {
        if(useGravity)
            AddForce(mass * PhyQuantities::GRAVITY_ACCELERATION * Vector2::down, ForceMode::Sustain);

        ApplyImpulse(force * dt);
        ApplyImpulseMoment(torque * dt);

        force = Vector2::zero;
        torque = 0.0f;
    }
    bool Rigidbody::StepTransform(float dt)
    {
        if(LessNear(dt, 0.0f))
            return false;
        if(linearVelocity == Vector2::zero && Near(angularVelocity, 0.0f))
            return false;
        
        linearVelocity *= Exp(-linearDrag * dt);
        angularVelocity *= Exp(-angularDrag * dt);

        _transform->LocalPosition(_transform->Position() + linearVelocity * dt);
        _transform->LocalRotation(_transform->Rotation() + angularVelocity * dt);
        return true;
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