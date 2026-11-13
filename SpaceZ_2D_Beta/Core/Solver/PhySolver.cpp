#include "Core/Solver/PhySolver.h"
#include "Core/Object/PhyObject/Collider.h"
#include "Core/Object/PhyObject/Rigidbody.h"
#include "Core/Debug/DebugFormat.h"
#include "Core/Math/Vector2.h"
using namespace Core::Math;
using namespace std;

namespace Core::Solver
{
    void PhySolver::Solve(vector<SolveReq>& reqs)
    {
        _dynamicBodies.Clear();
        _staticBodies.Clear();

        vector<IslandDivisionReq> islandReqs;
        for(auto& req : reqs)
            CreateReq(req, islandReqs);

        auto islands = _islandDivider.Islands(islandReqs);
        for(auto& island : islands)
            IterationSolve(island);
    }
    void PhySolver::ForEach(const function<void(const RigidbodyHandle&, const SolverRigidbody&)>& func)
    {
        _dynamicBodies.ForEach([&](const SolverBodyHandle& solverHandle, SolverRigidbody& solverBody)
        {
            if(!solverHandle)
                return;
            RigidbodyHandle handle(solverHandle);
            func(handle, solverBody);
        });
    }

    std::pair<SolverBodyHandle ,SolverRigidbody*> PhySolver::CreateSolverBody(const Collider* collider, const Rigidbody* body)
    {
        if(!collider || !collider->Handle())
            return {SolverBodyHandle::null, nullptr};
        
        SolverBodyHandle handle = SolverBodyHandle::null;
        SolverRigidbody* solverBodyPtr = nullptr;
        if(body && body->Handle())
        {
            handle = SolverBodyHandle(body->Handle());
            if(_dynamicBodies.TryGet(handle, solverBodyPtr))
                return {handle, solverBodyPtr};

            auto poolPtr = _bodyPool.Get();
            solverBodyPtr = poolPtr.GetPtr();
            if(!solverBodyPtr || !_dynamicBodies.TryAdd(handle, move(poolPtr)))
            {
                Debug::LogErrorFormat
                (
                    "【物理解算器】动态解算刚体创建失败",
                    "SolveBody:", handle,
                    "Body:", body->Handle()
                );
                return {SolverBodyHandle::null, nullptr};
            }

            solverBodyPtr->SetBody(body->mass, body->inertia);
            solverBodyPtr->position = body->Position();
            solverBodyPtr->rotation = body->Rotation();
            solverBodyPtr->linearVelocity = body->linearVelocity;
            solverBodyPtr->angularVelocity = body->angularVelocity;
        }
        else
        {
            handle = SolverBodyHandle(collider->Handle());
            if(_staticBodies.TryGet(handle, solverBodyPtr))
                return {handle, solverBodyPtr};
            
            auto poolPtr = _bodyPool.Get();
            solverBodyPtr = poolPtr.GetPtr();
            if(!solverBodyPtr || !_staticBodies.TryAdd(handle, move(poolPtr)))
            {
                Debug::LogErrorFormat
                (
                    "【物理解算器】静态解算刚体创建失败",
                    "SolveBody:", handle,
                    "Collider:", collider->Handle()
                );
                return {SolverBodyHandle::null, nullptr};
            }

            solverBodyPtr->SetBody();
            solverBodyPtr->position = collider->Position();
            solverBodyPtr->rotation = collider->Rotation();
            solverBodyPtr->linearVelocity = Vector2::zero;
            solverBodyPtr->angularVelocity = 0.0f;
        }
        return {handle, solverBodyPtr};
    }
    void PhySolver::CreateReq(SolveReq& req, vector<IslandDivisionReq>& ans)
    {
        if(!req)
            return;
        auto firstSolveBody = CreateSolverBody(req.colliders.first, req.bodies.first);
        auto secondSolveBody = CreateSolverBody(req.colliders.second, req.bodies.second);

        if(!firstSolveBody.first || !firstSolveBody.second
        || !secondSolveBody.first || !secondSolveBody.second)
            return;
        
        SolverPair solverPair
        {
            req.infos,
            firstSolveBody.second,
            secondSolveBody.second
        };

        auto& colliderA = *req.colliders.first;
        auto& colliderB = *req.colliders.second;
        solverPair.SetContact
        (
            { colliderA.staticFriction, colliderB.staticFriction },
            { colliderA.dynamicFriction, colliderB.dynamicFriction},
            { colliderA.elasticity, colliderB.elasticity}
        );

        IslandDivisionReq islandReq
        {
            firstSolveBody.first,
            secondSolveBody.first,
            solverPair,
        };
        ans.emplace_back(move(islandReq));
    }
    void PhySolver::IterationSolve(vector<SolverPair>& pairs)
    {
        if(pairs.empty())
            return;

        for(auto& p : pairs)
            p.Precompute();
        for(int i = 0; i < _iteration; i++)
        {
            for(auto& p : pairs)
            {
                p.ApplyNormalImpulse();
                p.ApplyTangentImpulse();
            }
            for(auto& p : pairs)
                p.ApplyPositionCorrection();
        }
    }
}