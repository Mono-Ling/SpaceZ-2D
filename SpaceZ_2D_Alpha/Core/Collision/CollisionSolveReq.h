#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"

namespace Core::Collision
{
    struct CollisionSolveReq
    {
        Rigidbody* firstBody = nullptr;
        Collider* firstCollider = nullptr;

        Rigidbody* secondBody = nullptr;
        Collider* secondCollider = nullptr;
        
        float depth = 0;
        Math::Vector2 point = Math::Vector2::zero;
        Math::Vector2 normal = Math::Vector2::zero; // first -> second

        // 刚体本地坐标系下（含缩放）碰撞点
        Math::Vector2 firstAnchorPoint = Math::Vector2::zero;
        Math::Vector2 secondAnchorPoint = Math::Vector2::zero;

        inline void SetCollisionInfo(const Math::Vector2& p, const Math::Vector2& n, float d)
        {
            this->point = p;
            this->normal = n;
            this->depth = d;
        }
        inline void SetCollisionPoint(const Math::Vector2& first, const Math::Vector2& second)
        {
            firstAnchorPoint = first;
            secondAnchorPoint = second;
        }

        constexpr bool IsEnable() const { return firstCollider && secondCollider; }
    };
}
