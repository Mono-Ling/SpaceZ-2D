#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"
#include "Math/Transform.h"

namespace Core
{
    enum class ForceType : int
    {
        Impulse,
        Sustain,
    };

    class Rigidbody
    {
    private:
        RigidbodyHandle _handle;

    public:
        bool useGravity = true;
        float mass = 1.0f;
        float inertia = 1.0f;

        float linearDrag = 0.0f;
        float angularDrag = 0.05f;

        float angularVelocity = 0.0f;
        Math::Vector2 linearVelocity;

        Math::Vector2 force;
        float torque = 0.0f;

        Math::Transform transform;

        Rigidbody(const RigidbodyHandle& handle) : _handle(handle) {}

        inline RigidbodyHandle Handle() const { return _handle; }
        inline Math::Vector2 Position() const { return transform.Position(); }
        inline float Rotation() const { return transform.Rotation(); }

        void AddForce(const Math::Vector2& force, ForceType type);
        void AddTorque(float torque, ForceType type);
        void Step(float dt);
        /// @brief transform步进
        /// @param dt delta time
        /// @return 是否产生变化
        bool StepTransform(float dt);

    private:
        void ApplyImpulse(const Math::Vector2& impulse);
        void ApplyImpulseMoment(float moment);
    };
}