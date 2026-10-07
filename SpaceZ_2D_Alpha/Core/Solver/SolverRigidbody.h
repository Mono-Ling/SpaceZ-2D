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

    private:
        RigidbodyHandle _handle;

    public:
        Math::Vector2 position;
        Math::Vector2 linearVelocity;

        SolverRigidbody() : _handle(RigidbodyHandle::null), invMass(0), invInertia(0) {}
        SolverRigidbody(const RigidbodyHandle& handle, float m, float i) : _handle(handle), invMass(1 / m), invInertia(1 / i) {}

        Math::Vector2 GetRealVelocity(const Math::Vector2& r) const;
        void ApplyImpulse(const Math::Vector2& p);
        void ApplyImpulseMoment(float m);
    };
}