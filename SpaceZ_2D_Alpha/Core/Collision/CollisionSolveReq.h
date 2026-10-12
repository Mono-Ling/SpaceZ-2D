#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"
#include <utility>

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
        std::pair<Math::Vector2, Math::Vector2> anchorPoint;

        inline void SetCollisionInfo(const Math::Vector2& p, const Math::Vector2& n, float d)
        {
            this->point = p;
            this->normal = n;
            this->depth = d;
        }

        constexpr bool IsEnable() const { return firstCollider && secondCollider; }
    };
}
