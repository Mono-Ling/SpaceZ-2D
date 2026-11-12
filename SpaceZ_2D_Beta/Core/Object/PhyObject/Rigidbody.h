#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Math/Vector2.h"
#include "Core/Object/PhyObject/Transform.h"
#include <memory>

namespace Core
{
    enum class ForceMode : int
    {
        Impulse,
        Sustain,
    };

    class Rigidbody
    {
    private:
        RigidbodyHandle _handle;
        std::shared_ptr<Transform> _transform;

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

        Rigidbody(const RigidbodyHandle& handle)
        : _handle(handle), _transform(std::make_shared<Transform>()) {}

        inline RigidbodyHandle Handle() const { return _handle; }
        inline Transform& GetTransform() const { return *_transform; }
        inline Math::Vector2 Position() const { return _transform->Position(); }
        inline float Rotation() const { return _transform->Rotation(); }

        void AddForce(const Math::Vector2& force, ForceMode mode);
        void AddTorque(float torque, ForceMode type);
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