#pragma once
#include "Core/Collision/Info/RayHit.h"
#include "Core/Object/PhyObject/PhyObject.h"

namespace Core
{
    struct RaycastHit
    {
        ColliderHandle collider;
        Math::Vector2 point;
        Math::Vector2 normal;
        float distance = -1.0f;

        RaycastHit() = default;
        RaycastHit(const ColliderHandle& handle, const Collision::RayHit& hit)
        : collider(handle), point(hit.point), normal(hit.normal), distance(hit.distance) {}
    };
}