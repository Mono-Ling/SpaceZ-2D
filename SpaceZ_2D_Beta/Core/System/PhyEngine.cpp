#include "Core/System/PhySystem.h"
#include "Core/Collision/NarrowPhase/NarrowPhase.h"
#include "Core/Math/Math.h"
#include "Core/Debug/DebugFormat.h"
#include <algorithm>
using namespace Core::Collision;
using namespace Core::Solver;
using namespace Core::Math;
using namespace std;

namespace Core
{
    int PhySystem::GetDirtyBodyCount() const
    {
        return static_cast<int>(_dirtyBodies.size());
    }
    void PhySystem::ForeachDirtyBodies(const function<void(const RigidbodyHandle&, const Vector2&, float, const Vector2&, float)>& func) const
    {
        for(auto& handle : _dirtyBodies)
        {
            auto body = TryGetBody(handle);
            if(!body)
                continue;
            auto& transform = body->GetTransform();
            func(handle, transform.Position(), transform.Rotation(), body->linearVelocity, body->angularVelocity);
        }
    }
    int PhySystem::GetCollisionPairCount() const
    {
        return static_cast<int>(_collisionPairs.size());
    }
    const std::vector<CollisionPair>& PhySystem::CollisionPairs() const
    {
        return _collisionPairs;
    }
    void PhySystem::Step(float dt)
    {
        _dirtyBodies.clear();
        _collisionPairs.clear();
        if(dt <= 0.0f)
            return;
        float step = dt / _subStep;
        for(int i = 0; i < _subStep; i++)
            SubStep(step);
    }

    ConvexDetectReq PhySystem::CreateConvexDetectReq(const ColliderHandle& collider) const
    {
        auto colliderPtr = TryGetCollider<Collider>(collider);
        if(!colliderPtr)
            return {};
        return colliderPtr->CreateConvexDetectReq();
    }
    ContinueDetectReq PhySystem::CreateContinueDetectReq(const BroadPhaseCollisionPair& collisionPair) const
    {
        ContinueDetectReq req;
        if(!collisionPair)
            return req;
        req.first = CreateConvexDetectReq(collisionPair.colliders.first);
        req.second = CreateConvexDetectReq(collisionPair.colliders.second);
        auto firstBody = TryGetBody(collisionPair.bodies.first);
        auto secondBody = TryGetBody(collisionPair.bodies.second);

        if(firstBody)
        {
            req.linearVelocity.first = firstBody->linearVelocity;
            req.angularVelocity.first = firstBody->angularVelocity;
        }
        else
        {
            req.linearVelocity.first = Vector2::zero;
            req.angularVelocity.first = 0.0f;
        }
        if(secondBody)
        {
            req.linearVelocity.second = secondBody->linearVelocity;
            req.angularVelocity.second = secondBody->angularVelocity;
        }
        else
        {
            req.linearVelocity.second = Vector2::zero;
            req.angularVelocity.second = 0.0f;
        }
        return req;
    }
    SolveReq PhySystem::CreateSolveReq(const CollisionPair& collisionPair) const
    {
        SolveReq req;
        req.colliders = 
        {
            TryGetCollider<Collider>(collisionPair.colliders.first),
            TryGetCollider<Collider>(collisionPair.colliders.second),
        };
        req.bodies = 
        {
            TryGetBody(collisionPair.bodies.first),
            TryGetBody(collisionPair.bodies.second),
        };
        req.infos = collisionPair.infos;
        return req;
    }
    void PhySystem::SubStep(float dt)
    {
        if(dt <= 0)
            return;
        vector<SolveReq> solveReqs;
        int count = 0;
        while (dt > 0.0f && count++ < MAX_CCD_ITERATION)
        {
            float toi = dt;
            auto pairs = ContinuousCollisionStep(toi);
            for(auto& p : pairs)
            {
                solveReqs.emplace_back(CreateSolveReq(p));
                _collisionPairs.emplace_back(p);
            }

            TransformStep(toi);
            BodyStep(toi);
            SolveStep(solveReqs);
            solveReqs.clear();
            dt -= toi;
        }
        if(dt > 0.0f)
        {
            TransformStep(dt);
            BodyStep(dt);
        }
        
        auto pairs = DiscreteCollisionStep();
        for(auto& p : pairs)
        {
            solveReqs.emplace_back(CreateSolveReq(p));
            _collisionPairs.emplace_back(p);
        }
        SolveStep(solveReqs);
    }
    vector<CollisionPair> PhySystem::ContinuousCollisionStep(float& time)
    {
        if(time < 0.0f)
            return {};
        SynDirtyCollider();
        SynDiscreteBroadPhase();
        SynContinuousBroadPhase(time);

        auto broadPairs = _broadPhase.CollisionPairs();
        vector<pair<float, CollisionPair>> narrowPairs;
        for(auto& bp : broadPairs)
        {
            if(!IsContinuousCollider(bp.colliders.first)
            && !IsContinuousCollider(bp.colliders.second))
                continue;

            auto req = CreateContinueDetectReq(bp);
            if(!req)
                continue;
            float toi = time;
            vector<CollisionInfo> infos;
            if(!ContinueCollision(req, toi, infos) || toi < 0.0f || toi > time)
                continue;
            CollisionPair collisionPair;
            collisionPair.colliders = bp.colliders;
            collisionPair.bodies = bp.bodies;
            collisionPair.infos = move(infos);
            narrowPairs.emplace_back(toi, collisionPair);
        }

        if(narrowPairs.empty())
            return {};
        sort(narrowPairs.begin(), narrowPairs.end(), [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        });

        float minStep = time * MIN_CCD_STEP;
        auto it = upper_bound(narrowPairs.begin(), narrowPairs.end(), minStep,
            [](float val, const pair<float, CollisionPair>& elem)
            {
                return val < elem.first;
            });
        time = it != narrowPairs.end() ? it->first : time;

        vector<CollisionPair> ans;
        for(auto& p : narrowPairs)
            if(p.first <= time)
                ans.emplace_back(move(p.second));
            else
                break;

        return ans;
    }
    vector<CollisionPair> PhySystem::DiscreteCollisionStep()
    {
        SynDirtyCollider();
        SynDiscreteBroadPhase();
        SynContinuousBroadPhase(0.0f);
        auto broathPairs = _broadPhase.CollisionPairs();
        if(broathPairs.empty())
            return {};

        vector<CollisionPair> ans;
        Simplex simplex;
        for(auto& bp : broathPairs)
        {
            if(!bp)
                continue;
            auto first = CreateConvexDetectReq(bp.colliders.first);
            auto second = CreateConvexDetectReq(bp.colliders.second);
            
            simplex.count = 0;
            if(!GJK(first, second, simplex))
                continue;
            auto info = EPA(first, second, simplex.points);
            auto infos = ContactManifold(first, second, info);

            CollisionPair collisionPair;
            collisionPair.colliders = bp.colliders;
            collisionPair.bodies = bp.bodies;
            collisionPair.infos = move(infos);

            ans.emplace_back(collisionPair);
        }
        return ans;
    }
    void PhySystem::BodyStep(float dt)
    {
        if(dt <= 0)
            return;
        _bodies.ForEach([&](const RigidbodyHandle& handle, Rigidbody& body)
        {
            body.Step(dt);

            _dirtyBodies.insert(handle);
        });
    }
    void PhySystem::TransformStep(float dt)
    {
        if(dt <= 0)
            return;
        _bodies.ForEach([&](const RigidbodyHandle& handle, Rigidbody& body)
        {
            if(!body.StepTransform(dt))
                return;
            
            SynDynamicCollider(handle);
            _dirtyBodies.insert(handle);
        });
    }
    void PhySystem::SolveStep(vector<SolveReq>& reqs)
    {
        _solver.Solve(reqs);
        _solver.ForEach([&](const RigidbodyHandle& handle, const SolverRigidbody& solverBody)
        {
            auto bodyPtr = TryGetBody(handle);
            if(!bodyPtr)
                return;
            
            Rigidbody& body = *bodyPtr;
            auto& transform = body.GetTransform();
            body.linearVelocity = solverBody.linearVelocity;
            body.angularVelocity = solverBody.angularVelocity;

            transform.LocalPosition(solverBody.position);
            transform.LocalRotation(solverBody.rotation);

            SynDynamicCollider(handle);
            _dirtyBodies.insert(handle);
        });
    }
}