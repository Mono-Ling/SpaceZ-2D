#include "System/PhySystem.h"
#include "Object/Collider/BoxCollider.h"
#include "Object/Collider/SphereCollider.h"
#include "Object/Collider/CapsuleCollider.h"
#include "Solver/PhyQuantities.h"
#include "Math/Math.h"
using namespace Core::Math;
using namespace Core::Collision;
using namespace Core::Solver;
using namespace std;

namespace Core
{
    PhySystem::~PhySystem()
    {
        vector<RigidbodyHandle> tempBodies;
        _bodies.Foreach([&](const RigidbodyHandle& handle, Rigidbody* ptr)
        {
            tempBodies.push_back(handle);
        });
        for(auto& handle : tempBodies)
            DestroyBody(handle);
        
        vector<ColliderHandle> tempColliders;
        _colliders.Foreach([&](const ColliderHandle& handle, Collider* ptr)
        {
            tempColliders.push_back(handle);
        });
        for(auto& handle : tempColliders)
            DestroyCollider(handle);
    }
    RigidbodyHandle PhySystem::CreateBody()
    {
        auto handle = _bodies.CreateHandle();
        auto body = new Rigidbody(handle);
        _bodies.AddObject(body, handle);

        _dynamicColliders.insert({handle, {}});
        return handle;
    }
    ColliderHandle PhySystem::CreateCollider(ColliderType type, const RigidbodyHandle& body)
    {
        Collider* collider = nullptr;
        auto handle = _colliders.CreateHandle();
        switch(type)
        {
            case ColliderType::Box:
                collider = new BoxCollider(handle);
            break;

            case ColliderType::Sphere:
                collider = new SphereCollider(handle);
            break;

            case ColliderType::Capsule:
                collider = new CapsuleCollider(handle);
            break;

            default:
                return ColliderHandle::null;
        }
        _colliders.AddObject(collider, handle);

        auto bodyPtr = TryGetBody(body);
        // 否则采用默认值静态刚体
        if(body != RigidbodyHandle::null && bodyPtr)
        {
            collider->SetBody(body);
            _dynamicColliders[body].insert(handle);
            collider->SetParentTransform(bodyPtr->Transform());
        }
        _dirtyColliderSet.insert(handle);
        return handle;
    }
    void PhySystem::DestroyBody(const RigidbodyHandle& handle)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        _bodies.TryRemove(handle);
        delete body;
        
        vector<ColliderHandle> temp(_dynamicColliders[handle].begin(), _dynamicColliders[handle].end());
        for(auto& collider : temp)
            DestroyCollider(collider);
        _dynamicColliders.erase(handle);
    }
    void PhySystem::DestroyCollider(const ColliderHandle& handle)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        auto body = collider->Body();
        _colliders.TryRemove(handle);
        _breadthPhase.Remove(handle);
        delete collider;

        auto it = _dynamicColliders.find(body);
        if(it == _dynamicColliders.end())
            return;
        it->second.erase(handle);
    }
    void PhySystem::SetBody(const RigidbodyHandle& handle, float mass, float inertia)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->mass = Max(Epsilon, mass);
        body->inertia = Max(Epsilon, inertia);
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
        collider->staticFriction = Max(0.0f, sf);
        collider->dynamicFriction = Max(0.0f, df);
        collider->elasticity = Clamp01(e);
    }
    void PhySystem::SetColliderRigidbody(const ColliderHandle& handle, const RigidbodyHandle& body)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;

        auto it = _dynamicColliders.find(collider->Body());
        if(it != _dynamicColliders.end())
            it->second.erase(handle);

        auto newBody = TryGetBody(body);
        collider->SetBody(newBody ? body : RigidbodyHandle::null);
        if(newBody)
        {
            collider->SetParentTransform(newBody->Transform());
            _dynamicColliders[body].insert(handle);
        }
        else
            collider->SetParentTransform(Transform());

        _dirtyColliderSet.insert(handle);
    }
    void PhySystem::UpdateTransform(const RigidbodyHandle& handle, const Vector2& pos, float rot, const Vector2& scale)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->Position(pos);
        body->Rotation(rot);
        body->Scale(scale);

        auto& colliderSet = _dynamicColliders[handle];
        for(auto& c : colliderSet)
        {
            auto collider = TryGetCollider<Collider>(c);
            if(!collider)
                continue;
            collider->SetParentTransform(body->Transform());

            _dirtyColliderSet.insert(c);
        }
    }
    void PhySystem::UpdateTransform(const ColliderHandle& handle, const Vector2& pos, float rot, const Vector2& scale)
    {
        auto collider = TryGetCollider<Collider>(handle);
        if(!collider)
            return;
        collider->SetLocalTransform(pos, rot, scale);

        _dirtyColliderSet.insert(handle);
    }
    void PhySystem::SetBoxCollider(const ColliderHandle& handle, const Vector2& ex)
    {
        auto box = TryGetCollider<BoxCollider>(handle);
        if(!box)
            return;
        box->SetBox(ex);

        _dirtyColliderSet.insert(handle);
    }
    void PhySystem::SetSphereCollider(const ColliderHandle& handle, float r)
    {
        auto sphere = TryGetCollider<SphereCollider>(handle);
        if(!sphere)
            return;
        sphere->SetSphere(r);

        _dirtyColliderSet.insert(handle);
    }
    void PhySystem::SetCapsuleCollider(const ColliderHandle& handle, float h, float r)
    {
        auto capsule = TryGetCollider<CapsuleCollider>(handle);
        if(!capsule)
            return;
        capsule->SetCapsule(h, r);

        _dirtyColliderSet.insert(handle);
    }
    void PhySystem::SynBreadthPhase()
    {
        if(_dirtyColliderSet.empty())
            return;
        for(auto& handle : _dirtyColliderSet)
        {
            auto collider = TryGetCollider<Collider>(handle);
            if(!collider)
                continue;
            BVHNode* node = nullptr;
            if(!_breadthPhase.TryGetObj(handle, node))
                _breadthPhase.Insert(handle, collider->Body(), collider->GetBound());
            else
                _breadthPhase.UpdateCollider(handle, collider->Body(), collider->GetBound());
        }
        _dirtyColliderSet.clear();
    }
    void PhySystem::Step(float dt)
    {
        ClearDirtyBodies();
        if(dt < 0)
            return;
            
        ApplyForce(dt);
        ApplyTorque(dt);

        float subStep = dt / _subStepCount;
        for(int i = 0; i < _subStepCount; i++)
            SubStep(subStep);
    }
    int PhySystem::GetCollisionPairCount() const
    {
        return _narrowPhase.GetCollisionPairs().size();
    }
    void PhySystem::ForeachCollisionPairs(const function<void(CollisionPair)>& func) const
    {
        auto pairs = _narrowPhase.GetCollisionPairs();
        for(auto& p : pairs)
            func(p);
    }
    int PhySystem::GetDirtyBodyCount() const
    {
        return _dirtyBodySet.size();
    }
    void PhySystem::ForeachDirtyBodies(const function<void(const RigidbodyHandle&, const Vector2&, float, const Vector2&, float)>& func) const
    {
        for(auto& handle : _dirtyBodySet)
        {
            auto body = TryGetBody(handle);
            if(!body)
                continue;
            func(handle, body->Position(), body->Rotation(), body->linearVelocity, body->angularVelocity);
        }
    }
    void PhySystem::AddForce(const RigidbodyHandle& handle, const Vector2& force)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->force += force;
    }
    void PhySystem::AddTorque(const RigidbodyHandle& handle, float torque)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        body->torque += torque;
    }

    template<typename T>
    requires std::derived_from<T, Collider>
    T* PhySystem::TryGetCollider(const ColliderHandle& handle) const
    {
        Collider* ptr;
        if(!_colliders.TryGet(handle, ptr) || !ptr)
            return nullptr;
        return dynamic_cast<T*>(ptr);
    }
    template<>
    Collider* PhySystem::TryGetCollider<Collider>(const ColliderHandle& handle) const
    {
        Collider* ptr;
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
    void PhySystem::ClearDirtyBodies() { _dirtyBodySet.clear(); }
    void PhySystem::SynDynamicCollider(const RigidbodyHandle& body)
    {
        auto bodyPtr = TryGetBody(body);
        if(!bodyPtr)
            return;
        auto& colliderSet = _dynamicColliders[body];
        for(auto& c : colliderSet)
        {
            auto collider = TryGetCollider<Collider>(c);
            if(!collider)
                continue;
            collider->SetParentTransform(bodyPtr->Transform());

            _dirtyColliderSet.insert(c);
        }
    }

    void PhySystem::AddGravity()
    {
        _bodies.Foreach([&](const RigidbodyHandle& handle, Rigidbody* ptr)
        {
            if(!ptr)
                return;
            AddForce(handle, Vector2::down * ptr->mass * PhyQuantities::GRAVITY_ACCELERATION);
        });
    }
    void PhySystem::ApplyForce(float dt)
    {
        _bodies.Foreach([&](const RigidbodyHandle& handle, Rigidbody* ptr)
        {
            if(!ptr)
                return;
            _solver.AddForce(ptr, ptr->force, dt);
            ptr->force = Vector2::zero;

            _dirtyBodySet.insert(handle);
        });
    }
    void PhySystem::ApplyTorque(float dt)
    {
        _bodies.Foreach([&](const RigidbodyHandle& handle, Rigidbody* ptr)
        {
            if(!ptr)
                return;
            _solver.AddTorque(ptr, ptr->torque, dt);
            ptr->torque = 0.0f;

            _dirtyBodySet.insert(handle);
        });
    }
    void PhySystem::SubStep(float dt)
    {
        dt = Max(0.0f, dt);

        AddGravity();
        ApplyForce(dt);
        ApplyTorque(dt);

        CollisionStep();
        SolveStep(dt);
        TransformStep(dt);
    }
    void PhySystem::CollisionStep()
    {
        SynBreadthPhase();
        auto pairs = _breadthPhase.GetCollisionPairs();
        vector<NarrowPhaseDetectPair> npPairs;
        for(auto& p : pairs)
        {
            auto first = TryGetCollider<Collider>(p.first.collider);
            auto second = TryGetCollider<Collider>(p.second.collider);
            if(!first || !second)
                continue;
            npPairs.push_back({first->GetConvexDetectReq(), second->GetConvexDetectReq(), p});
        }
        _narrowPhase.SetDetectPairs(npPairs);
    }
    void PhySystem::SolveStep(float dt)
    {
        dt = Max(0.0f, dt);
        auto pairs = _narrowPhase.GetCollisionPairs();
        if(pairs.empty())
            return;
        vector<CollisionSolveReq> reqs;
        for(auto& p : pairs)
        {
            CollisionSolveReq req;
            req.firstCollider = TryGetCollider<Collider>(p.first.collider);
            req.secondCollider = TryGetCollider<Collider>(p.second.collider);
            req.firstBody = TryGetBody(p.first.body);
            req.secondBody = TryGetBody(p.second.body);

            req.SetCollisionInfo(p.point, p.normal, p.depth);
            req.anchorPoint = p.anchorPoint;

            if(req.IsEnable())
                reqs.push_back(req);
        }
        if(reqs.empty())
            return;
        _solver.SolveStep(reqs, dt);

        _solver.Foreach([&](const RigidbodyHandle& handle, const SolverRigidbody* sBody)
        {
            if(!sBody)
                return;
            auto body = TryGetBody(handle);
            if(!body)
                return;
            body->Position(sBody->position);
            body->Rotation(sBody->angle);

            body->linearVelocity = sBody->linearVelocity;
            body->angularVelocity = sBody->angularVelocity;

            _dirtyBodySet.insert(handle);
            SynDynamicCollider(handle);
        });
    }
    void PhySystem::TransformStep(float dt)
    {
        dt = Max(0.0f, dt);
        _bodies.Foreach([&](const RigidbodyHandle& handle, Rigidbody* ptr)
        {
            if(!ptr)
                return;
            ptr->Step(dt);

            _dirtyBodySet.insert(handle);
            SynDynamicCollider(handle);
        });
    }
}