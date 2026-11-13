#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Collision/Info/CollisionInfo.h"
#include <utility>
#include <vector>

namespace Core::Solver
{
    struct SolveReq
    {
        std::pair<Collider*, Collider*> colliders;
        std::pair<Rigidbody*, Rigidbody*> bodies;
        std::vector<Collision::CollisionInfo> infos;

        inline explicit operator bool() const
        {
            return colliders.first && colliders.second && (bodies.first || bodies.second)
                && !infos.empty();
        }
    };
}