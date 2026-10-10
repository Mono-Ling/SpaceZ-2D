#pragma once
#include "Math/Vector2.h"
#include "Object/Object.h"

namespace Core::Solver
{
    struct SolverRigidbody
    {
    public:
        float invInertia;
        float invMass;

        float angle;
        float angularVelocity;

        Math::Vector2 position;
        Math::Vector2 linearVelocity;

        SolverRigidbody() : invMass(0), invInertia(0) {}
        SolverRigidbody(float m, float i) : invMass(1 / m), invInertia(1 / i) {}

        void Reset();
        void Reset(float m, float i);
        Math::Vector2 GetRealVelocity(const Math::Vector2& r) const;
        void ApplyImpulse(const Math::Vector2& p);
        void ApplyImpulseMoment(float m);

        void ApplyPositionImpulse(const Math::Vector2& p);
        void ApplyAngleImpulse(float m);

        Math::Vector2 PointLocalToWorld(const Math::Vector2& point);
    };
}