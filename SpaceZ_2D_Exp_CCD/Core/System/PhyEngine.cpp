#include "System/PhySystem.h"
#include <algorithm>
using namespace Core::Collision;
using namespace Core::Math;
using namespace Core::Object;
using namespace Core::Solver;
using namespace std;

namespace Core
{
    void PhySystem::Step(float dt)
    {
        ClearDirtyBodies();
        if(dt <= 0.0f)
            return;
        dt /= _subStepCount;
        for(int i = 0; i < _subStepCount; i++)
            SubStep(dt);
    }
    int PhySystem::GetDirtyBodyCount() const { return static_cast<int>(_dirtyBodySet.size()); }
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

    void PhySystem::ClearDirtyBodies() { _dirtyBodySet.clear(); }
    void PhySystem::SynDynamicCollider(const RigidbodyHandle& handle)
    {
        auto body = TryGetBody(handle);
        if(!body)
            return;
        auto& transform = body->transform;
        auto colliders = _dynamicColliders[handle];
        for(auto& collider : colliders)
        {
            auto ptr = TryGetCollider<Collider>(collider);
            if(!ptr)
                continue;
            ptr->transform.SetParentTransform(transform);
        }
    }
    ContinuousDetectReq PhySystem::CreateContinuousDetectReq(const CollisionPair& p) const
    {
        if(!p)
            return {};
        auto firstCollider = TryGetCollider<Collider>(p.collider.first);
        auto secondCollider = TryGetCollider<Collider>(p.collider.second);
        if(!firstCollider || !secondCollider)
            return {};
        auto firstBody = TryGetBody(p.body.first);
        auto secondBody = TryGetBody(p.body.second);
        return ContinuousDetectReq
        {
            firstCollider->CreateConvexDetectReq(),
            secondCollider->CreateConvexDetectReq(),
            {
                firstBody != nullptr ? firstBody->linearVelocity : Vector2::zero,
                secondBody != nullptr ? secondBody->linearVelocity : Vector2::zero,
            },
            {
                firstBody != nullptr ? firstBody->angularVelocity : 0.0f,
                secondBody != nullptr ? secondBody->angularVelocity : 0.0f,
            }
        };
    }
    SolveReq PhySystem::CreateSolveReq(const CollisionPair& p, vector<CollisionInfo>& infos) const
    {
        if(!p)
            return {};
        auto firstCollider = TryGetCollider<Collider>(p.collider.first);
        auto secondCollider = TryGetCollider<Collider>(p.collider.second);

        auto firstBody = TryGetBody(p.body.first);
        auto secondBody = TryGetBody(p.body.second);
        SolveReq req;
        req.colliders = {firstCollider, secondCollider};
        req.bodies = {firstBody, secondBody};
        req.collisionInfos = move(infos);
        return req;
    }
    void PhySystem::SubStep(float dt)
    {
        while (dt >= 0)
        {
            SynBroadPhase(dt);
            auto time = dt;
            auto reqs = CollisionStep(time);
            dt -= time;
            if(dt <= 0)
                break;
            BodyStep(time);
            TransformStep(time);
            SolveStep(reqs);
        }
    }
    void PhySystem::SynBroadPhase(float dt)
    {
        for(auto& p : _dynamicColliders)
        {
            auto body = TryGetBody(p.first);
            if(!body)
                continue;
            auto dx = body->linearVelocity * dt;
            auto da = body->angularVelocity * dt;

            for(auto& handle : p.second)
            {
                auto collider = TryGetCollider<Collider>(handle);
                if(!collider)
                    continue;
                auto sweep = collider->SweepBound(dx, da);
                if(!_broadPhase.Update(handle, collider->Body(), sweep))
                    _broadPhase.Insert(handle, collider->Body(), sweep);
            }
        }

        if(_dirtyStaticColliderSet.empty())
            return;
        for(auto& handle : _dirtyStaticColliderSet)
        {
            auto collider = TryGetCollider<Collider>(handle);
            if(!collider)
                continue;
            auto sweep = collider->SweepBound(Vector2::zero, 0.0f);
            if(!_broadPhase.Update(handle, collider->Body(), sweep))
                _broadPhase.Insert(handle, collider->Body(), sweep);
        }
        _dirtyStaticColliderSet.clear();
    }
    vector<SolveReq> PhySystem::CollisionStep(float& time)
    {
        auto pairs = _broadPhase.GetCollisionPairs();
        vector<pair<float, SolveReq>> reqs;
        vector<CollisionInfo> infos;
        for(auto& p : pairs)
        {
            float t = time;
            auto req = CreateContinuousDetectReq(p);
            if(!ContinuousCollision(req, t, infos) || t < 0 || t > time)
                continue;
            reqs.emplace_back(pair<float, SolveReq>(t, CreateSolveReq(p, infos)));
        }
        vector<SolveReq> ans;
        if(reqs.empty())
            return ans;
        sort(reqs.begin(), reqs.end(), [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        });
        time = reqs.front().first;
        for(auto& req : reqs)
            if(Near(req.first, time))
                ans.push_back(req.second);
        return ans;
    }
    void PhySystem::BodyStep(float dt)
    {
        _bodies.ForEach([&](const RigidbodyHandle& handle, Rigidbody& body)
        {
            body.Step(dt);

            _dirtyBodySet.insert(handle);
        });
    }
    void PhySystem::TransformStep(float dt)
    {
        _bodies.ForEach([&](const RigidbodyHandle& handle, Rigidbody& body)
        {
            body.StepTransform(dt);

            SynDynamicCollider(handle);
            _dirtyBodySet.insert(handle);
        });
    }
    void PhySystem::SolveStep(const vector<SolveReq>& reqs)
    {
        if(reqs.empty())
            return;
        _solver.PhySolve(reqs);
        _solver.ForEachBodies([&](const RigidbodyHandle& handle, SolverRigidbody* const solverBody)
        {
            if(!handle || !solverBody)
                return;
            auto body = TryGetBody(handle);
            if(!body)
                return;
            body->linearVelocity = solverBody->linearVelocity;
            body->angularVelocity = solverBody->angularVelocity;
            body->transform.Position(solverBody->position);
            body->transform.Rotation(solverBody->angle);

            SynDynamicCollider(handle);
            _dirtyBodySet.insert(handle);
        });
    }
}