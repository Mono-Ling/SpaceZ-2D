#pragma once
#include "Object/ObjectMgr.h"
#include "Object/Rigidbody.h"
#include "Object/Collider.h"
#include "Object/Collider/ColliderType.h"
#include "CollisionDetection/BVH/BVHTree.h"
#include "CollisionDetection/NarrowPhase/NarrowPhase.h"
#include "Solver/PhySolver.h"
#include "Math/Math.h"
#include <functional>
#include <unordered_map>
#include <unordered_set>

#define COL Collision::
#define SOL Solver::
#define MAT Math::

namespace Core
{
    class PhySystem
    {
        int _subStepCount;

        /// @brief 应用层-与刚体相对位置更改
        std::unordered_set<ColliderHandle> _dirtyColliderSet;
        /// @brief 物理层-刚体位置更改
        std::unordered_set<RigidbodyHandle> _dirtyBodySet;
        std::unordered_map<RigidbodyHandle, std::unordered_set<ColliderHandle>> _dynamicColliders;
    
        ObjectMgr<Rigidbody> _bodies;
        ObjectMgr<Collider> _colliders;

        COL NarrowPhase _narrowPhase;
        COL BVHTree _breadthPhase;
        
        SOL PhySolver _solver;

    public:
        PhySystem(int subStep, int iteration) : _subStepCount(MAT Max(1, subStep)), _solver({MAT Abs(iteration)}) {}
        ~PhySystem();

        RigidbodyHandle CreateBody();
        ColliderHandle CreateCollider(ColliderType type, const RigidbodyHandle& body);
        void DestroyBody(const RigidbodyHandle& handle);
        void DestroyCollider(const ColliderHandle& handle);

        void SetBody(const RigidbodyHandle& handle, float mass, float inertia);
        void SetBodyDrag(const RigidbodyHandle& handle, float lDrag, float aDrag);
        void SetVelocity(const RigidbodyHandle& handle, const MAT Vector2& lv, float av);

        void SetCollider(const ColliderHandle& handle, float sf, float df, float e);
        void SetColliderRigidbody(const ColliderHandle& handle, const RigidbodyHandle& body);

        void UpdateTransform(const RigidbodyHandle& handle, const MAT Vector2& pos, float rot, const MAT Vector2& scale);
        void UpdateTransform(const ColliderHandle& handle, const MAT Vector2& pos, float rot, const MAT Vector2& scale);

        void SetBoxCollider(const ColliderHandle& handle, const MAT Vector2& ex);
        void SetSphereCollider(const ColliderHandle& handle, float r);
        void SetCapsuleCollider(const ColliderHandle& handle, float h, float r);

        void SynBreadthPhase();

        void Step(float dt);

        int GetCollisionPairCount() const;
        void ForeachCollisionPairs(const std::function<void(COL CollisionPair)>& func) const;

        int GetDirtyBodyCount() const;
        /// @brief 遍历更改刚体
        /// @param func (handle, position, rotation, linearV, angularV)
        void ForeachDirtyBodies(const std::function<void(const RigidbodyHandle&, const MAT Vector2&, float, const MAT Vector2&, float)>& func) const;

        void AddForce(const RigidbodyHandle& handle, const MAT Vector2& force);
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

        void AddGravity();
        void ApplyForce(float dt);
        void ApplyTorque(float dt);

        void SubStep(float dt);
        void CollisionStep();
        void SolveStep(float dt);
        void TransformStep(float dt);
    };
}
#undef COL
#undef SOL
#undef MAT