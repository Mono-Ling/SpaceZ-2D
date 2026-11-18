#pragma once
#include "Core/Object/PhyObject/Collider/ConvexCollider.h"
#include "Core/Collision/Convex/Line.h"
#include "Core/Collision/Convex/Box.h"
#include "Core/Collision/Convex/Sphere.h"
#include "Core/Collision/Convex/Capsule.h"

namespace Core
{
    using LineCollider = ConvexCollider<Collision::Line>;
    using BoxCollider = ConvexCollider<Collision::Box>;
    using SphereCollider = ConvexCollider<Collision::Sphere>;
    using CapsuleCollider = ConvexCollider<Collision::Capsule>;
}