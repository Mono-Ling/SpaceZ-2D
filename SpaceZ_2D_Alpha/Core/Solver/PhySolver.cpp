#include "Solver/PhySolver.h"
#include "Object/Rigidbody.h"
#include "Object/Collider.h"
#include "Math/Vector2.h"
using namespace std;
using namespace Core::Collision;
using namespace Core::Math;

namespace Core::Solver
{
    void SetSolverRigidbody(SolverRigidbody* solverBody, Rigidbody* body)
    {
        if(!solverBody || !body)
            return;
        solverBody->angle = body->angle;
        solverBody->position = body->position;

        solverBody->angularVelocity = body->angularVelocity;
        solverBody->linearVelocity = body->linearVelocity;
    }
    void SetSolverRigidbody(SolverRigidbody* solverBody, Collider* collider)
    {
        if(!solverBody || !collider)
            return;
        solverBody->angle = collider->angle;
        solverBody->position = collider->position;

        solverBody->angularVelocity = 0.0f;
        solverBody->linearVelocity = Vector2::zero;
    }

    PhySolver::~PhySolver()
    {
        while(!_solverBodyBuffer.empty())
        {
            auto ptr = _solverBodyBuffer.top();
            _solverBodyBuffer.pop();
            delete ptr;
        }
        for(auto& p : _solverBodies)
            if(p.second) delete p.second;
        for(auto& p : _staticSolverBodies)
            if(p.second) delete p.second;
    }
    void PhySolver::SolveStep(const std::vector<Collision::CollisionSolveReq>& pairs, float dt)
    {
        ClearTemp();
        auto solverPairs = CreateSolverPairs(pairs);
        auto islands = _islandDivider.IslandDivision(solverPairs);

        for(auto& island : islands)
            ImpulseIteration(island, dt);
    }
    void PhySolver::Foreach(function<void(const RigidbodyHandle&, const SolverRigidbody* const)> func)
    {
        for(auto& p : _solverBodies)
            func(p.first, p.second);
    }
    void PhySolver::ClearTemp()
    {
        for(auto& p : _solverBodies)
            if(p.second) _solverBodyBuffer.push(p.second);
        for(auto& p : _staticSolverBodies)
            if(p.second) _solverBodyBuffer.push(p.second);
        _solverBodies.clear();
        _staticSolverBodies.clear();
    }

    SolverRigidbody* PhySolver::CreateSolverRigidbody(float m, float i)
    {
        if(!_solverBodyBuffer.empty())
        {
            auto body = _solverBodyBuffer.top();
            _solverBodyBuffer.pop();
            body->Reset(m,i);
            return body;
        }
        return new SolverRigidbody(m, i);
    }
    SolverRigidbody* PhySolver::CreateSolverRigidbody()
    {
        if(!_solverBodyBuffer.empty())
        {
            auto body = _solverBodyBuffer.top();
            _solverBodyBuffer.pop();
            body->Reset();
            return body;
        }
        return new SolverRigidbody();
    }
    vector<SolverPair> PhySolver::CreateSolverPairs(const vector<CollisionSolveReq>& pairs)
    {
        vector<SolverPair> ans;
        for(auto& p : pairs)
        {
            if(!p.IsEnable())
                continue;

            SolverPair solverPair(p.point, p.normal, {p.firstAnchorPoint, p.secondAnchorPoint});
            SolverRigidbody* a = nullptr;
            SolverRigidbody* b = nullptr;

            if(p.firstCollider->Body() == RigidbodyHandle::null || !p.firstBody)
            {
                auto it = _staticSolverBodies.find(p.firstCollider->Handle());
                if(it == _staticSolverBodies.end())
                {
                    a = CreateSolverRigidbody();
                    _staticSolverBodies.insert({p.firstCollider->Handle(), a});
                }
                else
                    a = it->second;
                
                SetSolverRigidbody(a, p.firstCollider);
            }
            else
            {
                auto it = _solverBodies.find(p.firstBody->Handle());
                if(it == _solverBodies.end())
                {
                    a = CreateSolverRigidbody(p.firstBody->mass, p.firstBody->inertia);
                    _solverBodies.insert({p.firstBody->Handle(), a});
                }
                else
                    a = it->second;

                SetSolverRigidbody(a, p.firstBody);
            }

            if(p.secondCollider->Body() == RigidbodyHandle::null || !p.secondBody)
            {
                auto it = _staticSolverBodies.find(p.secondCollider->Handle());
                if(it == _staticSolverBodies.end())
                {
                    b = CreateSolverRigidbody();
                    _staticSolverBodies.insert({p.secondCollider->Handle(), b});
                }
                else
                    b = it->second;

                SetSolverRigidbody(b, p.secondCollider);
            }
            else
            {
                auto it = _solverBodies.find(p.secondBody->Handle());
                if(it == _solverBodies.end())
                {
                    b = CreateSolverRigidbody(p.secondBody->mass, p.secondBody->inertia);
                    _solverBodies.insert({p.secondBody->Handle(), b});
                }
                else
                    b = it->second;

                SetSolverRigidbody(b, p.secondBody);
            }
            solverPair.SetRigidbody(a, b);
            solverPair.SetContact(*p.firstCollider, *p.secondCollider);
            ans.push_back(solverPair);
        }
        return ans;
    }
    void PhySolver::ImpulseIteration(vector<SolverPair>& island, float step) const
    {
        if(island.empty())
            return;
        for(int i = 0; i < _iteration; i++)
            for(auto& p : island)
            {
                p.ApplyNormalImpulse();
                p.ApplyTangentImpulse(step);
            }
        for(int i = 0; i < _iteration; i++)
            for(auto& p : island)
                p.ApplyPositionCorrection();
    }
}