#pragma once
#include "Solver/SolverPair.h"
#include <unordered_map>
#include <vector>

namespace Core::Solver
{
    struct IslandDivider
    {
    private:
        std::unordered_map<SolverRigidbody*, SolverRigidbody*> _unionFind;

    public:
        std::vector<std::vector<SolverPair>> GetIslands(const std::vector<SolverPair>& pairs);

    private:
        void InitUnionFind(const std::vector<SolverPair>& pairs);
        SolverRigidbody* const FindRoot(SolverRigidbody* ptr);
        void AddPair(const SolverPair& p);
        SolverRigidbody* const IsSameRoot(SolverRigidbody* a, SolverRigidbody* b);
    };
}