#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"

namespace Core::Collision
{
    struct CollisionPair
    {
        Rigidbody* firstBody = nullptr;
        Collider* firstCollider = nullptr;

        Rigidbody* secondBody = nullptr;
        Collider* secondCollider = nullptr;
        
        float depth = 0;
        Math::Vector2 point = Math::Vector2::zero;
        Math::Vector2 normal = Math::Vector2::zero; // first -> second

        constexpr void SetCollisionRigidbody(Rigidbody* first, Rigidbody* second)
        {
            firstBody = first;
            secondBody = second;
        }
        constexpr void SetCollisionCollider(Collider* first, Collider* second)
        {
            firstCollider = first;
            secondCollider = second;
        }
        inline void SetCollisionInfo(Math::Vector2 point, Math::Vector2 normal, float depth)
        {
            this->point = point;
            this->normal = normal;
            this->depth = depth;
        }

        constexpr bool IsEnable() const { return firstCollider && secondCollider; }
    };
}
