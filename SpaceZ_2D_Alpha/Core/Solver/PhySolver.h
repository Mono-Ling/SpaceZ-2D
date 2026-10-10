#pragma once
#include "Solver/SolverPair.h"
#include "Solver/SolverRigidbody.h"
#include "Solver/IslandDivider.h"
#include "Collision/CollisionSolveReq.h"
#include "Object/Object.h"
#include <unordered_map>
#include <vector>
#include <stack>
#include <functional>

namespace Core::Solver
{
    class PhySolver
    {
    private:
        int _iteration;
        std::stack<SolverRigidbody*> _solverBodyBuffer;
        std::unordered_map<RigidbodyHandle, SolverRigidbody*> _solverBodies;
        std::unordered_map<ColliderHandle, SolverRigidbody*> _staticSolverBodies;
        IslandDivider _islandDivider;

    public:
        PhySolver(int iteration) : _iteration(iteration),_islandDivider(IslandDivider()) {}
        ~PhySolver();

        void SolveStep(const std::vector<Collision::CollisionSolveReq>& pairs, float dt);
        void Foreach(std::function<void(const RigidbodyHandle&, const SolverRigidbody* const)> func);
        void ClearTemp();

    private:
        SolverRigidbody* CreateSolverRigidbody(float m, float i);
        SolverRigidbody* CreateSolverRigidbody();
        std::vector<SolverPair> CreateSolverPairs(const std::vector<Collision::CollisionSolveReq>& pairs);
        void ImpulseIteration(std::vector<SolverPair>& island, float step) const;
    };
}