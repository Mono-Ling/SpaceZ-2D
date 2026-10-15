#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"
#include "Math/Transform.h"
#include "CollisionDetection/Geometry/Bound.h"

namespace Core
{
    class Collider
    {
    public:
        float staticFriction;
        float dynamicFriction;
        float elasticity;

    private:
        RigidbodyHandle _body = RigidbodyHandle::null;
        ColliderHandle _handle;

        Collision::Bound _bound;

    protected:
        Math::Transform _transform;
        
    public:
        Collider(const ColliderHandle& handle) : _handle(handle) {}
        virtual ~Collider() = default;
        
        constexpr ColliderHandle Handle() const { return _handle; }
        constexpr RigidbodyHandle Body() const { return _body; }
        constexpr void SetBody(const RigidbodyHandle& body) { _body = body; }

        inline Math::Vector2 Position() const { return _transform.WorldPosition(); }
        inline float Rotation() const { return _transform.WorldRotation(); }

        inline Collision::Bound GetBound() const { return _bound; }

        inline void SetLocalTransform(const Math::Vector2& pos, float rot, const Math::Vector2& scale)
        {
            _transform.SetLocalPosition(pos);
            _transform.SetLocalRotation(rot);
            _transform.SetLocalScale(scale);
            UpdateBound();
        }
        inline void SetParentTransform(const Math::Transform& parent)
        {
            _transform.SetParentTransform(parent);
            UpdateBound();
        }
        
    protected:
        virtual Math::Vector2 Support(const Math::Vector2& dir) const = 0;
    
    private:
        inline void UpdateBound()
        {
            Math::Vector2 minV
            {
                Support(Math::Vector2::left).x,
                Support(Math::Vector2::down).y
            };
            Math::Vector2 maxV
            {
                Support(Math::Vector2::right).x,
                Support(Math::Vector2::up).y
            };
            _bound = Collision::CreateBound(minV, maxV);
        }
    };
}