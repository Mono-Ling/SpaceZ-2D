#pragma once
#include "Object/Object.h"
#include "Collision/Info/CollisionInfo.h"
#include <utility>
#include <vector>

namespace Core::Solver
{
    struct SolveReq
    {
        std::pair<Collider*, Collider*> colliders;
        std::pair<Rigidbody*, Rigidbody*> bodies;

        std::vector<Collision::CollisionInfo> collisionInfos;

        SolveReq() : colliders({nullptr, nullptr}), bodies({nullptr, nullptr}) {}
        inline operator bool() const { return colliders.first && colliders.second; }
    };
}