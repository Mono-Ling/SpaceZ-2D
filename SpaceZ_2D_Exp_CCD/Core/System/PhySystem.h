#pragma once
#include "Object/Rigidbody.h"
#include "Object/Collider/ColliderType.h"
#include "Object/Collider/BoxCollider.h"
#include "Object/Collider/SphereCollider.h"
#include "Object/Collider/CapsuleCollider.h"
#include "Object/ObjectMgr.h"
#include "Collision/BVH/BVHTree.h"
#include "Collision/NarrowPhase/NarrowPhase.h"
#include "Solver/PhySolver.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

namespace Core
{
    class PhySystem
    {
        int _subStepCount;

        /// @brief 物理层-刚体位置更改
        std::unordered_set<RigidbodyHandle> _dirtyBodySet;
        std::unordered_map<RigidbodyHandle, std::unordered_set<ColliderHandle>> _dynamicColliders;
        /// @brief 应用层-与刚体相对位置更改
        std::unordered_set<ColliderHandle> _dirtyStaticColliderSet;

        Object::ObjectMgr<Rigidbody> _bodies;
        Object::ObjectMgr<Collider> _colliders;

        Collision::BVHTree _broadPhase;

        Solver::PhySolver _solver;

    public:
        PhySystem(int subStep, int iteration) : _subStepCount(subStep), _solver({iteration}) {}

        RigidbodyHandle CreateBody();
        ColliderHandle CreateCollider(const RigidbodyHandle& body, ColliderType type);
        void DestoryBody(const RigidbodyHandle& handle);
        void DestoryCollider(const ColliderHandle& handle);

        void SetBody(const RigidbodyHandle& handle, float mass, float inertia, bool useGravity);
        void SetBodyDrag(const RigidbodyHandle& handle, float lDrag, float aDrag);
        void SetVelocity(const RigidbodyHandle& handle, const Math::Vector2& lv, float av);

        void SetCollider(const ColliderHandle& handle, float sf, float df, float e);
        void SetColliderRigidbody(const ColliderHandle& handle, const RigidbodyHandle& body);

        void UpdateTransform(const RigidbodyHandle& handle, const Math::Vector2& pos, float rot, const Math::Vector2& scale);
        void UpdateTransform(const ColliderHandle& handle, const Math::Vector2& pos, float rot, const Math::Vector2& scale);

        void SetBoxCollider(const ColliderHandle& handle, const Math::Vector2& ex);
        void SetSphereCollider(const ColliderHandle& handle, float r);
        void SetCapsuleCollider(const ColliderHandle& handle, float r, float h);

        void Step(float dt);

        int GetDirtyBodyCount() const;
        /// @brief 遍历更改刚体
        /// @param func (handle, position, rotation, linearV, angularV)
        void ForeachDirtyBodies(const std::function<void(const RigidbodyHandle&, const Math::Vector2&, float, const Math::Vector2&, float)>& func) const;

        void AddForce(const RigidbodyHandle& handle, const Math::Vector2& force);
        void AddTorque(const RigidbodyHandle& handle, float torque);

    private:
        template<typename T>
        requires std::derived_from<T, Collider>
        T* TryGetCollider(const ColliderHandle& handle) const;

        template<>
        Collider* TryGetCollider<Collider>(const ColliderHandle& handle) const;
        Rigidbody* TryGetBody(const RigidbodyHandle& handle) const;

        void ClearDirtyBodies();
        void SynDynamicCollider(const RigidbodyHandle& body);
        Collision::ContinuousDetectReq CreateContinuousDetectReq(const Collision::CollisionPair& p) const;
        Solver::SolveReq CreateSolveReq(const Collision::CollisionPair& p, std::vector<Collision::CollisionInfo>& infos) const;

        void SubStep(float dt);
        void SynBroadPhase(float dt);
        std::vector<Solver::SolveReq> CollisionStep(float& time);
        void BodyStep(float dt);
        void TransformStep(float dt);
        void SolveStep(const std::vector<Solver::SolveReq>& reqs);
    };
}