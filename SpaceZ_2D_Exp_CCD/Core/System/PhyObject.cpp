#include "System/PhySystem.h"

namespace Core
{
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