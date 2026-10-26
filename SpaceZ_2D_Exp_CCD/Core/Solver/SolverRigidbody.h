#pragma once
#include "Math/Vector2.h"

namespace Core::Solver
{
    class SolverRigidbody
    {
    public:
        float invInertia;
        float invMass;

        float angle;
        float angularVelocity;

        Math::Vector2 position;
        Math::Vector2 linearVelocity; 

        void SetBody();
        void SetBody(float mass, float inertia);

        Math::Vector2 GetRealVelocity(const Math::Vector2& r) const;
        void ApplyImpulse(const Math::Vector2& p);
        void ApplyImpulseMoment(float m);

        void ApplyPositionImpulse(const Math::Vector2& p);
        void ApplyAngleImpulse(float m);

        Math::Vector2 PointLocalToWorld(const Math::Vector2& point) const;
        Math::Vector2 PointWorldToLocal(const Math::Vector2& point) const;
    };
}