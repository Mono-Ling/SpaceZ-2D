#include "Solver/PhySolver.h"
#include "Debug/Debug.h"
#include "Math/Vector2.h"
using namespace Core::Math;
using namespace Core::Object;
using namespace std;

namespace Core::Solver
{
    void PhySolver::PhySolve(const vector<SolveReq>& reqs)
    {
        ClearTemp();
        vector<SolverPair> total;
        for(auto& req : reqs)
        {
            auto curr = CreateSolverPair(req);
            if(curr)
                total.emplace_back(move(curr));
        }
        if(total.empty())
            return;

        auto islands = _islandDivider.GetIslands(total);
        for(auto& island : islands)
            ImpulseIteration(island);
    }
    void PhySolver::ForEachBodies(function<void(const RigidbodyHandle&, SolverRigidbody* const)> func) const
    {
        for(auto& p : _dynamicBodies)
        {
            auto ptr = p.second.GetPtr();
            if(!ptr || !p.first)
                continue;
            func(p.first, ptr);
        }
    }
    void PhySolver::ClearTemp()
    {
        _dynamicBodies.clear();
        _staticBodies.clear();
    }

    SolverRigidbody* PhySolver::CreateSolverBody(Collider* collider, Rigidbody* body)
    {
        if(!collider || !collider->Handle())
            return nullptr;

        if(body && collider->Body() != body->Handle())
        {
            Debug::LogWarning("【物理解算器】碰撞体与刚体无关，构建解算刚体失败");
            return nullptr;
        }

        SolverRigidbody* solverBody = nullptr;
        if(body && body->Handle())
        {
            auto it = _dynamicBodies.find(body->Handle());
            if(it == _dynamicBodies.end())
            {
                auto newPoolPtr = _bodyPool.Get();
                if(!newPoolPtr)
                {
                    Debug::LogError("【物理解算器】解算刚体创建失败");
                    return nullptr;
                }

                solverBody = newPoolPtr.GetPtr();
                _dynamicBodies.emplace(body->Handle(), move(newPoolPtr));

                solverBody->SetBody(body->mass, body->inertia);
                solverBody->position = body->Position();
                solverBody->angle = body->Rotation();
                solverBody->linearVelocity = body->linearVelocity;
                solverBody->angularVelocity = body->angularVelocity;
            }
            else
                solverBody = it->second.GetPtr();
        }
        else
        {
            auto it = _staticBodies.find(collider->Handle());
            if(it == _staticBodies.end())
            {
                auto newPoolPtr = _bodyPool.Get();
                if(!newPoolPtr)
                {
                    Debug::LogError("【物理解算器】解算刚体创建失败");
                    return nullptr;
                }

                solverBody = newPoolPtr.GetPtr();
                _staticBodies.emplace(collider->Handle(), move(newPoolPtr));

                solverBody->SetBody();
                solverBody->position = collider->Position();
                solverBody->angle = collider->Rotation();
                solverBody->linearVelocity = Vector2::zero;
                solverBody->angularVelocity = 0.0f;
            }
            else
                solverBody = it->second.GetPtr();
        }
        return solverBody;
    }
    SolverPair PhySolver::CreateSolverPair(const SolveReq& req)
    {
        if(!req || req.collisionInfos.empty())
            return {};
        auto bodyA = CreateSolverBody(req.colliders.first, req.bodies.first);
        auto bodyB = CreateSolverBody(req.colliders.second, req.bodies.second);

        if(!bodyA || !bodyB)
        {
            Debug::LogWarning("【物理解算器】解算刚体构造失败，解算对创建失败");
            return {};
        }

        SolverPair ans(req.collisionInfos);
        ans.SetBody(bodyA, bodyB);
        ans.SetContact
        (
            {req.colliders.first->staticFriction, req.colliders.second->staticFriction},
            {req.colliders.first->dynamicFriction, req.colliders.second->dynamicFriction},
            {req.colliders.first->elasticity, req.colliders.second->elasticity}
        );

        return ans;
    }
    void PhySolver::ImpulseIteration(vector<SolverPair>& island) const
    {
        if(island.empty())
            return;

        for(auto& p : island)
            p.InitSolverPair();

        for(int i = 0; i < _iteration; i++)
            for(auto& p : island)
            {
                p.ApplyNormalImpulse();
                p.ApplyTangentImpulse();
            }

        for(int i = 0; i < _iteration; i++)
            for(auto& p : island)
                p.ApplyPositionCorrection();
    }
}