#pragma once
#include "Core/Object/Handle.h"
#include "Core/Math/Vector2.h"

namespace Core::Solver
{
    class SolverRigidbody
    {
    public:
        float invMass = 0.0f;
        float invInertia = 0.0f;

        float rotation = 0.0f;
        float angularVelocity = 0.0f;

        Math::Vector2 position;
        Math::Vector2 linearVelocity;

        void SetBody();
        void SetBody(float m, float i);

        Math::Vector2 RealVelocity(const Math::Vector2& r) const;

        void ApplyImpulse(const Math::Vector2& impulse);
        void ApplyImpulseMoment(float moment);

        void ApplyPositionImpulse(const Math::Vector2& p);
        void ApplyAngleImpulse(float m);

        Math::Vector2 PointLocalToWorld(const Math::Vector2& point) const;
        Math::Vector2 PointWorldToLocal(const Math::Vector2& point) const;
    };

    using SolverBodyHandle = Handle<SolverRigidbody>;
}