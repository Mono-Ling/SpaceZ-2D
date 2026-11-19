#include "Core/System/PhySystem.h"
#include "Core/Object/PhyObject/Collider/ConvexCollider.h"
#include "Core/Math/Math.h"
#include "Core/Debug/DebugFormat.h"
using namespace Core::Collision;
using namespace Core::Math;
using namespace std;

namespace Core
{
    RigidbodyHandle PhySystem::CreateBody()
    {
        auto handle = _bodies.PeekNextHandle();
        if(!_bodies.TryAdd(handle, make_unique<Rigidbody>(handle)))
        {
            Debug::LogError("【PhySystem-Object】刚体创建失败");
            return RigidbodyHandle::null;
        }
        _dynamicColliders[handle];
        return handle;
    }
    ColliderHandle PhySystem::CreateCollider(ColliderType type)
    {
        auto handle = _colliders.PeekNextHandle();
        unique_ptr<Collider> collider;
        switch(type)
        {
        case ColliderType::Line:
            collider = make_unique<LineCollider>(handle);
            break;

        case ColliderType::Box:
            collider = make_unique<BoxCollider>(handle);
            break;

        case ColliderType::Sphere:
            collider = make_unique<SphereCollider>(handle);
            break;

        case ColliderType::Capsule:
            collider = make_unique<CapsuleCollider>(handle);
            break;

        default:
            Debug::LogError("【PhySystem-Object】碰撞体创建失败，无效碰撞体类型");
            return ColliderHandle::null;
        }
        if(!_colliders.TryAdd(handle, move(collider)))
        {
            Debug::LogError("【PhySystem-Object】碰撞体创建失败");
            return ColliderHandle::null;
        }
        _dirtyCollider.insert(handle);
        return handle;
    }
    bool PhySystem::DestroyBody(const RigidbodyHandle& handle)
    {
        if(!_bodies.TryRemove(handle))
            return false;

        auto it = _dynamicColliders.find(handle);
        if(it != _dynamicColliders.end())
            for(auto& collider : it->second)
            {
                _colliders.TryRemove(collider);
                _broadPhase.Remove(collider);

                _dirtyCollider.insert(collider);
            }
        _dynamicColliders.erase(handle);
        return true;
    }
    bool PhySystem::DestroyCollider(const ColliderHandle& handle)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return false;
        auto body = collider->Body();
        if(body)
        {
            auto it = _dynamicColliders.find(body);
            if(it != _dynamicColliders.end())
                it->second.erase(handle);
        }

        _dirtyCollider.insert(handle);

        _broadPhase.Remove(handle);
        return _colliders.TryRemove(handle);
    }
    void PhySystem::SetColliderBody(const ColliderHandle& handle, const RigidbodyHandle& body)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        auto oldBody = collider->Body();
        if(oldBody == body)
            return;

        if(oldBody)
        {
            auto it = _dynamicColliders.find(oldBody);
            if(it != _dynamicColliders.end())
                it->second.erase(handle);
        }
        auto bodyPtr = TryGetBody(body);
        if(body && bodyPtr)
        {
            auto it = _dynamicColliders.find(body);
            if(it != _dynamicColliders.end())
                it->second.insert(handle);

            collider->transform.SetParent(bodyPtr->TransformPtr());
            collider->Body(body);
        }
        else
        {
            collider->transform.SetParent(nullptr);
            collider->Body(RigidbodyHandle::null);
        }
        _dirtyCollider.insert(handle);
    }
    void PhySystem::SetBodyProperty(const RigidbodyHandle& handle, float mass, float inertia, bool useGravity, float lDrag, float aDrag)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->mass = Max(mass, Epsilon);
        body->inertia = Max(inertia, Epsilon);
        body->useGravity = useGravity;
        body->linearDrag = Max(lDrag, 0.0f);
        body->angularDrag = Max(aDrag, 0.0f);
    }
    void PhySystem::SetVelocity(const RigidbodyHandle& handle, const Vector2& lv, float av)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->linearVelocity = lv;
        body->angularVelocity = av;
    }
    void PhySystem::SetColliderProperty(const ColliderHandle& handle, float sf, float df, float e)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        collider->staticFriction = Max(sf, 0.0f);
        collider->dynamicFriction = Max(df, 0.0f);
        collider->elasticity = Clamp01(e);
    }
    void PhySystem::SetColliderCollision(const ColliderHandle& handle, const LayerMask& layer, CollisionMode mode)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;

        collider->layer = layer;
        collider->collisionMode = mode;

        _dirtyCollider.insert(handle);
    }
    void PhySystem::UpdateTransform(const RigidbodyHandle& handle, const Vector2& pos, float rot, const Vector2& scale)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        auto& transform = body->GetTransform();

        transform.LocalPosition(pos);
        transform.LocalRotation(rot);
        transform.LocalScale(scale);

        auto it = _dynamicColliders.find(handle);
        if(it != _dynamicColliders.end())
            for(auto& collider : it->second)
                _dirtyCollider.insert(collider);
    }
    void PhySystem::UpdateTransform(const ColliderHandle& handle, const Vector2& pos, float rot, const Vector2& scale)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        auto& transform = collider->transform;
        transform.LocalPosition(pos);
        transform.LocalRotation(rot);
        transform.LocalScale(scale);

        _dirtyCollider.insert(handle);
    }
    void PhySystem::SetLineCollider(const ColliderHandle& handle, float length)
    {
        auto collider = TryGetCollider<LineCollider>(handle);
        if(!collider)
            return;
        collider->SetLine(length);
        
        _dirtyCollider.insert(handle);
    }
    void PhySystem::SetBoxCollider(const ColliderHandle& handle, const Vector2& ex)
    {
        auto collider = TryGetCollider<BoxCollider>(handle);
        if(!collider)
            return;
        collider->SetBox(ex);

        _dirtyCollider.insert(handle);
    }
    void PhySystem::SetSphereCollider(const ColliderHandle& handle, float r)
    {
        auto collider = TryGetCollider<SphereCollider>(handle);
        if(!collider)
            return;
        collider->SetSphere(r);

        _dirtyCollider.insert(handle);
    }
    void PhySystem::SetCapsuleCollider(const ColliderHandle& handle, float r, float h)
    {
        auto collider = TryGetCollider<CapsuleCollider>(handle);
        if(!collider)
            return;
        collider->SetCapsule(r, h);

        _dirtyCollider.insert(handle);
    }
    void PhySystem::AddForce(const RigidbodyHandle& handle, const Vector2& force, ForceMode mode)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->AddForce(force, mode);
    }
    void PhySystem::AddTorque(const RigidbodyHandle& handle, float torque, ForceMode mode)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->AddTorque(torque, mode);
    }

    template<typename T>
    requires std::derived_from<T, Collider>
    T* PhySystem::TryGetCollider(const ColliderHandle& handle) const
    {
        Collider* ptr = nullptr;
        if(!_colliders.TryGet(handle, ptr) || !ptr)
            return nullptr;
        return dynamic_cast<T*>(ptr);
    }
    template<>
    Collider* PhySystem::TryGetCollider<Collider>(const ColliderHandle& handle) const
    {
        Collider* ptr = nullptr;
        if(!_colliders.TryGet(handle, ptr))
            return nullptr;
        return ptr;
    }
    Rigidbody* PhySystem::TryGetBody(const RigidbodyHandle& handle) const
    {
        Rigidbody* ptr = nullptr;
        if(!_bodies.TryGet(handle, ptr))
            return nullptr;
        return ptr;
    }
    void PhySystem::SynDirtyCollider()
    {
        for(auto& handle : _dirtyCollider)
        {
            auto collider = TryGetCollider<Collider>(handle);
            if(!collider)
            {
                _continuousColliders.erase(handle);
                continue;
            }
            switch(collider->collisionMode)
            {
            case CollisionMode::Discrete:
                _dirtyDiscreteBound.insert(handle);
                _continuousColliders.erase(handle);
                break;

            case CollisionMode::Continuous:
                if(!_bodies.Contains(collider->Body()))
                {
                    _continuousColliders.erase(handle);
                    _dirtyDiscreteBound.insert(handle);
                }
                else
                    _continuousColliders.insert(handle);
                break;
            }
        }
        _dirtyCollider.clear();
        SynDiscreteBroadPhase();
    }
    void PhySystem::SynDiscreteBroadPhase()
    {
        for(auto& handle : _dirtyDiscreteBound)
        {
            auto collider = TryGetCollider<Collider>(handle);
            if(!collider)
                continue;
            auto bound = collider->SweepBound(Vector2::zero, 0.0f);
            if(!_broadPhase.Update(handle, collider->Body(), collider->layer, bound))
                _broadPhase.Insert(handle, collider->Body(), collider->layer, bound);
        }
        _dirtyDiscreteBound.clear();
    }
    void PhySystem::SynContinuousBroadPhase(float dt)
    {
        if(dt <= 0.0f)
            return;
        for(auto& handle : _continuousColliders)
        {
            auto collider = TryGetCollider<Collider>(handle);
            if(!collider)
                continue;
            auto body = TryGetBody(collider->Body());
            if(!body)
                continue;

            auto bound = collider->SweepBound(body->linearVelocity * dt, body->angularVelocity * dt);
            if(!_broadPhase.Update(handle, collider->Body(), collider->layer, bound))
                _broadPhase.Insert(handle, collider->Body(), collider->layer, bound);
        }
    }
}