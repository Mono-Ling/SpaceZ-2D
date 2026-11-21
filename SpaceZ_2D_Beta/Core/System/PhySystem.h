#pragma once
#include "Core/Object/PhyObject/Rigidbody.h"
#include "Core/Object/PhyObject/Collider.h"
#include "Core/Object/Container/ObjectMgr.h"
#include "Core/Collision/BroadPhase/BVHTree.h"
#include "Core/Solver/PhySolver.h"
#include "Core/Object/PhyObject/Collider/ColliderType.h"
#include "Core/Object/PhyObject/Collider/CollisionMode.h"
#include "Core/Collision/Layer/LayerMask.h"
#include "Core/Collision/Info/ConvexDetectReq.h"
#include "Core/Collision/Info/ContinueDetectReq.h"
#include "Core/System/Info/CollisionPair.h"
#include "Core/System/Info/RaycastHit.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace Core
{
    class PhySystem
    {
        static constexpr float MIN_CCD_STEP = 0.001f;
        static constexpr int MAX_CCD_ITERATION = 36;
        int _subStep = 1;

        Container::ObjectMgr<Rigidbody> _bodies;
        Container::ObjectMgr<Collider> _colliders;
        std::unordered_map<RigidbodyHandle, std::unordered_set<ColliderHandle>> _dynamicColliders;

        std::unordered_set<ColliderHandle> _dirtyDiscreteBound;
        std::unordered_set<ColliderHandle> _continuousColliders;
        std::unordered_set<ColliderHandle> _dirtyCollider;
        std::unordered_set<RigidbodyHandle> _dirtyBodies;
        std::vector<CollisionPair> _collisionPairs;

        Collision::BVHTree _broadPhase;
        Solver::PhySolver _solver;

    public:
        PhySystem(int subStep, int iteration) : _subStep(subStep > 1 ? subStep : 1), _solver(iteration) {}

        RigidbodyHandle CreateBody();
        ColliderHandle CreateCollider(ColliderType type);
        bool DestroyBody(const RigidbodyHandle& handle);
        bool DestroyCollider(const ColliderHandle& handle);
        void SetColliderBody(const ColliderHandle& handle, const RigidbodyHandle& body);

        void SetBodyProperty(const RigidbodyHandle& handle, float mass, float inertia, bool useGravity, float lDrag, float aDrag);
        void SetVelocity(const RigidbodyHandle& handle, const Math::Vector2& lv, float av);

        void SetColliderProperty(const ColliderHandle& handle, float sf, float df, float e);
        void SetColliderCollision(const ColliderHandle& handle, const Collision::LayerMask& layer, CollisionMode mode);

        void SetTransform(const RigidbodyHandle& handle, const Math::Vector2& pos, float rot, const Math::Vector2& scale);
        void SetTransform(const ColliderHandle& handle, const Math::Vector2& pos, float rot, const Math::Vector2& scale);
        
        void SetLineCollider(const ColliderHandle& handle, float length);
        void SetBoxCollider(const ColliderHandle& handle, const Math::Vector2& ex);
        void SetSphereCollider(const ColliderHandle& handle, float r);
        void SetCapsuleCollider(const ColliderHandle& handle, float r, float h);

        void AddForce(const RigidbodyHandle& handle, const Math::Vector2& force, ForceMode mode);
        void AddTorque(const RigidbodyHandle& handle, float torque, ForceMode mode);

        int GetDirtyBodyCount() const;
        /// @brief 遍历更改刚体
        /// @param func (handle, position, rotation, linearV, angularV)
        void ForeachDirtyBodies(const std::function<void(const RigidbodyHandle&, const Math::Vector2&, float, const Math::Vector2&, float)>& func) const;

        int GetCollisionPairCount() const;
        const std::vector<CollisionPair>& CollisionPairs() const;

        void Step(float dt);

        std::vector<ColliderHandle> OverlapBox(const Math::Vector2& center, const Math::Vector2& size, float rotation, const Collision::LayerMask& layer) const;
        std::vector<ColliderHandle> OverlapSphere(const Math::Vector2& center, float radius, float rotation, const Collision::LayerMask& layer) const;
        std::vector<ColliderHandle> OverlapCapsule(const Math::Vector2& center, float radius, float height, float rotation, const Collision::LayerMask& layer) const;

        bool Raycast(const Math::Vector2& origin, const Math::Vector2& direction, RaycastHit& hit, float maxDis, const Collision::LayerMask& layer) const;
        std::vector<RaycastHit> RaycastAll(const Math::Vector2& origin, const Math::Vector2& direction, float maxDis, const Collision::LayerMask& layer) const;

    private:
        template<typename T>
        requires std::derived_from<T, Collider>
        T* TryGetCollider(const ColliderHandle& handle) const;

        template<>
        Collider* TryGetCollider<Collider>(const ColliderHandle& handle) const;
        Rigidbody* TryGetBody(const RigidbodyHandle& handle) const;

        bool IsContinuousCollider(const ColliderHandle& handle) const;

        void SynDynamicCollider(const RigidbodyHandle& handle);
        void SynDirtyCollider();
        void SynDiscreteBroadPhase();
        void SynContinuousBroadPhase(float dt);

        Collision::ConvexDetectReq CreateConvexDetectReq(const ColliderHandle& collider) const;
        Collision::ContinueDetectReq CreateContinueDetectReq(const Collision::BroadPhaseCollisionPair& collisionPair) const;
        Solver::SolveReq CreateSolveReq(const CollisionPair& collisionPair) const;

        void SubStep(float dt);
        std::vector<CollisionPair> ContinuousCollisionStep(float& time);
        std::vector<CollisionPair> DiscreteCollisionStep();
        void BodyStep(float dt);
        void TransformStep(float dt);
        void SolveStep(std::vector<Solver::SolveReq>& reqs);

        std::vector<ColliderHandle> OverlapConvex(const Collision::Convex& convex, const Math::Vector2& center, float rotation, const Collision::LayerMask& layer) const;
    };
}