#pragma once
#include "Math/Vector2.h"
#include "Object/Collider.h"
#include "Solver/SolverRigidbody.h"

namespace Core::Solver
{
    struct SolverPair
    {
        SolverRigidbody* first;
        SolverRigidbody* second;
        
        float staticFriction;
        float dynamicFriction;
        float elasticity;

        float invEffMass;
        float normalImpulse; // first -> second

        Math::Vector2 point;
        Math::Vector2 normal; // first -> second

        SolverPair(const Math::Vector2& point, const Math::Vector2& normal)
        : point(point), normal(normal.Normalized()), normalImpulse(0), first(nullptr), second(nullptr) {}

        bool IsEnable() const;
        void SetRigidbody(SolverRigidbody* first, SolverRigidbody* second);
        void SetContact(const Collider& first, const Collider& second);

        void ApplyNormalImpulse();
        void ApplyTangentImpulse(float step);
    };
}