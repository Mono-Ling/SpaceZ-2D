#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"
#include "Math/Transform.h"
#include "Collision/Geometry/Bound.h"
#include "Collision/Convex/Convex.h"
#include "Collision/NarrowPhase/ConvexDetectReq.h"

namespace Core
{
    class Collider
    {
    private:
        ColliderHandle _handle;
        RigidbodyHandle _body = RigidbodyHandle::null;

    public:
        float staticFriction = 0;
        float dynamicFriction = 0;
        float elasticity = 0;

        Math::Transform transform;

        Collider(const ColliderHandle& handle) : _handle(handle) {}
        virtual ~Collider() = default;

        inline ColliderHandle Handle() const { return _handle; }
        inline RigidbodyHandle Body() const { return _body; }
        inline void Body(const RigidbodyHandle& body) { _body = body; }
        inline Math::Vector2 Position() const { return transform.Position(); }
        inline float Rotation() const { return transform.Rotation(); }

        Collision::Bound SweepBound(const Math::Vector2& dx, float da) const;
        Collision::ConvexDetectReq CreateConvexDetectReq() const;

    protected:
        virtual Math::Vector2 Support(const Math::Vector2& dir, const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2local) const = 0;
        virtual const Collision::Convex* ConvexPtr() const = 0;

    private:
        Collision::Bound BakeBound(const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2local) const;
    };
}