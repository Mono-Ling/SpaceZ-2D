#include "System/PhySystem.h"
#include "Math/Math.h"
#include "Debug/Debug.h"
#include <memory>
using namespace Core::Math;
using namespace std;

namespace Core
{
    RigidbodyHandle PhySystem::CreateBody()
    {
        auto handle = _bodies.PeekNextHandle();
        if(!_bodies.TryAdd(handle, make_unique<Rigidbody>(handle)))
        {
            Debug::LogError("【物理引擎-物理对象】刚体创建失败");
            return RigidbodyHandle::null;
        }
        _dynamicColliders.insert({handle, {}});
        return handle;
    }
    ColliderHandle PhySystem::CreateCollider(const RigidbodyHandle& body, ColliderType type)
    {
        auto handle = _colliders.PeekNextHandle();
        bool ans = false;
        switch (type)
        {
        case ColliderType::Box:
            ans = _colliders.TryAdd(handle, make_unique<BoxCollider>(handle));
            break;
        
        case ColliderType::Sphere:
            ans = _colliders.TryAdd(handle, make_unique<SphereCollider>(handle));
            break;

        case ColliderType::Capsule:
            ans = _colliders.TryAdd(handle, make_unique<CapsuleCollider>(handle));
            break;
        
        default:
            Debug::LogError("【物理引擎-物理对象】碰撞体创建失败，无效碰撞体类型");
            return ColliderHandle::null;
        }

        auto colliderPtr = TryGetCollider<Collider>(handle);
        auto bodyPtr = TryGetBody(body);
        if(!ans || !colliderPtr)
        {
            Debug::LogError("【物理引擎-物理对象】碰撞体创建失败");
            return ColliderHandle::null;
        }
        if(bodyPtr)
        {
            colliderPtr->Body(body);
            colliderPtr->transform.SetParentTransform(bodyPtr->transform);
            _dynamicColliders[body].insert(handle);
        }
        else
            _dirtyStaticColliderSet.insert(handle);
        return handle;
    }
    void PhySystem::DestroyBody(const RigidbodyHandle& handle)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        auto& colliderSet = _dynamicColliders[handle];
        for(auto& collider : colliderSet)
        {
            _broadPhase.Remove(collider);
            _colliders.TryRemove(collider);
        }
        _dynamicColliders.erase(handle);
        _bodies.TryRemove(handle);
    }
    void PhySystem::DestroyCollider(const ColliderHandle& handle)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        auto body = collider->Body();
        if(_bodies.Contains(body))
        {
            auto it = _dynamicColliders.find(body);
            if(it != _dynamicColliders.end())
                it->second.erase(handle);
            else
                Debug::LogWarning("【物理引擎-物理对象】动态碰撞体移除异常 Body:" + ToString(body));
        }
        else
            _dirtyStaticColliderSet.erase(handle);
        
        _broadPhase.Remove(handle);
        _colliders.TryRemove(handle);
    }
    void PhySystem::SetBody(const RigidbodyHandle& handle, float mass, float inertia, bool useGravity)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->mass = Max(mass, Epsilon);
        body->inertia = Max(inertia, Epsilon);
        body->useGravity = useGravity;
    }
    void PhySystem::SetBodyDrag(const RigidbodyHandle& handle, float lDrag, float aDrag)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->linearDrag = Clamp01(lDrag);
        body->angularDrag = Clamp01(aDrag);
    }
    void PhySystem::SetVelocity(const RigidbodyHandle& handle, const Vector2& lv, float av)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->linearVelocity = lv;
        body->angularVelocity = av;
    }
    void PhySystem::SetCollider(const ColliderHandle& handle, float sf, float df, float e)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        collider->staticFriction = sf;
        collider->dynamicFriction = df;
        collider->elasticity = Clamp01(e);
    }
    void PhySystem::SetColliderRigidbody(const ColliderHandle& handle, const RigidbodyHandle& body)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        auto oldBody = collider->Body();
        if(body == oldBody)
            return;
            
        if(_bodies.Contains(oldBody))
            _dynamicColliders[oldBody].erase(handle);
        else
            _dirtyStaticColliderSet.erase(handle);

        auto bodyPtr = TryGetBody(body);
        if(bodyPtr)
        {
            collider->Body(body);
            collider->transform.SetParentTransform(bodyPtr->transform);
            _dynamicColliders[body].insert(handle);
        }
        else
        {
            // null与找不到刚体均进入静态分支
            collider->Body(RigidbodyHandle::null);
            collider->transform.SetParentTransform(Transform());
            _dirtyStaticColliderSet.insert(handle);
        }
    }
    void PhySystem::UpdateTransform(const RigidbodyHandle& handle, const Vector2& pos, float rot, const Vector2& scale)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->transform.LocalPosition(pos);
        body->transform.LocalRotation(rot);
        body->transform.LocalScale(scale);

        SynDynamicCollider(handle);
    }
    void PhySystem::UpdateTransform(const ColliderHandle& handle, const Vector2& pos, float rot, const Vector2& scale)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        collider->transform.LocalPosition(pos);
        collider->transform.LocalRotation(rot);
        collider->transform.LocalScale(scale);

        if(!_bodies.Contains(collider->Body()))
            _dirtyStaticColliderSet.insert(handle);
    }
    void PhySystem::SetBoxCollider(const ColliderHandle& handle, const Vector2& ex)
    {
        auto box = TryGetCollider<BoxCollider>(handle);
        if(!box)
            return;
        box->SetBox(ex);

        if(!_bodies.Contains(box->Body()))
            _dirtyStaticColliderSet.insert(handle);
    }
    void PhySystem::SetSphereCollider(const ColliderHandle& handle, float r)
    {
        auto sphere = TryGetCollider<SphereCollider>(handle);
        if(!sphere)
            return;
        sphere->SetSphere(r);

        if(!_bodies.Contains(sphere->Body()))
            _dirtyStaticColliderSet.insert(handle);
    }
    void PhySystem::SetCapsuleCollider(const ColliderHandle& handle, float r, float h)
    {
        auto capsule = TryGetCollider<CapsuleCollider>(handle);
        if(!capsule)
            return;
        capsule->SetCapsule(r, h);

        if(!_bodies.Contains(capsule->Body()))
            _dirtyStaticColliderSet.insert(handle);
    }
    void PhySystem::AddForce(const RigidbodyHandle& handle, const Vector2& force, ForceType type)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->AddForce(force, type);
    }
    void PhySystem::AddTorque(const RigidbodyHandle& handle, float torque, ForceType type)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->AddTorque(torque, type);
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
        if(!_colliders.TryGet(handle, ptr) || !ptr)
            return nullptr;
        return ptr;
    }
    Rigidbody* PhySystem::TryGetBody(const RigidbodyHandle& handle) const
    {
        Rigidbody* ptr = nullptr;
        if(!_bodies.TryGet(handle, ptr) || !ptr)
            return nullptr;
        return ptr;
    }
}