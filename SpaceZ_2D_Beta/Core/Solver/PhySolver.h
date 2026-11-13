#pragma once
#include "Core/Solver/SolverRigidbody.h"
#include "Core/Solver/SolveReq.h"
#include "Core/Solver/IslandDivider.h"
#include "Core/Solver/SolverBodyBuffer.h"
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Object/Container/ObjectPool.h"
#include <vector>
#include <functional>
#include <utility>

namespace Core::Solver
{
    class PhySolver
    {
        int _iteration;
        Container::ObjectPool<SolverRigidbody> _bodyPool;
        SolverBodyBuffer _dynamicBodies;
        SolverBodyBuffer _staticBodies;
       
        IslandDivider _islandDivider;

    public:
        PhySolver(int iteration)
        : _iteration(iteration < 1 ? 1 : iteration), _bodyPool([](){ return new SolverRigidbody(); }) {}

        void Solve(std::vector<SolveReq>& reqs);
        void ForEach(const std::function<void(const RigidbodyHandle&, const SolverRigidbody&)>& func);

    private:
        std::pair<SolverBodyHandle ,SolverRigidbody*> CreateSolverBody(const Collider* collider, const Rigidbody* body);
        void CreateReq(SolveReq& req, std::vector<IslandDivisionReq>& ans);

        void IterationSolve(std::vector<SolverPair>& pairs);
    };
}