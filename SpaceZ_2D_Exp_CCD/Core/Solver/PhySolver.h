#pragma once
#include "Object/Rigidbody.h"
#include "Object/Collider.h"
#include "Object/Pool/ObjectPool.h"
#include "Solver/SolverRigidbody.h"
#include "Solver/IslandDivider.h"
#include "Solver/SolverPair.h"
#include "Solver/SolveReq.h"
#include <vector>
#include <unordered_map>
#include <functional>

namespace Core::Solver
{
    class PhySolver
    {
        int _iteration;
        Object::ObjectPool<SolverRigidbody> _bodyPool;
        IslandDivider _islandDivider;
        std::unordered_map<RigidbodyHandle, Object::PoolPtr<SolverRigidbody>> _dynamicBodies;
        std::unordered_map<ColliderHandle, Object::PoolPtr<SolverRigidbody>> _staticBodies;

    public:

        PhySolver(int iteration) : _iteration(iteration <= 0 ? 1 : iteration) {}
        PhySolver(const PhySolver& other) = delete;
        PhySolver(PhySolver&& other) = delete;

        PhySolver& operator=(const PhySolver& other) = delete;
        PhySolver& operator=(PhySolver&& other) = delete;
        
        void PhySolve(const std::vector<SolveReq>& reqs);
        void ForEachBodies(std::function<void(const RigidbodyHandle&, SolverRigidbody* const)> func) const;
        void ClearTemp();

    private:
        SolverRigidbody* CreateSolverBody(Collider* collider, Rigidbody* body);
        SolverPair CreateSolverPair(const SolveReq& req);
        void ImpulseIteration(std::vector<SolverPair>& island) const;
    };
}