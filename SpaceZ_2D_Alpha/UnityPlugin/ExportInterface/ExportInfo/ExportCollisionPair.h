#pragma once
#include "Collision/CollisionPair.h"

namespace Export
{
    struct ExportCollisionPair
    {
        Core::ColliderHandle firstCollider;
        Core::RigidbodyHandle firstBody;

        Core::ColliderHandle secondCollider;
        Core::RigidbodyHandle secondBody;

        float depth = 0.0f;
        Core::Math::Vector2 point;
        Core::Math::Vector2 normal;

        inline static ExportCollisionPair From(const Core::Collision::CollisionPair& p)
        {
            ExportCollisionPair ans;
            ans.firstCollider = p.first.collider;
            ans.firstBody = p.first.body;

            ans.secondCollider = p.second.collider;
            ans.secondBody = p.second.body;

            ans.depth = p.depth;
            ans.point = p.point;
            ans.normal = p.normal;
            return ans;
        }
    };
}