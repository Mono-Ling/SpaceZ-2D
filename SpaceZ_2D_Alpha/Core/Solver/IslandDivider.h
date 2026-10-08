#pragma once
#include "SolverRigidbody.h"
#include "Solver/SolverPair.h"
#include <unordered_map>
#include <vector>

namespace Core::Solver
{
    class IslandDivider
    {
    private:
        std::unordered_map<SolverRigidbody*, SolverRigidbody*> _unionSet;

    public:
        std::vector<std::vector<SolverPair>> IslandDivision(const std::vector<SolverPair>& pairs);

    private:
        void SetSolverPairs(const std::vector<SolverPair>& pairs);
        std::vector<std::vector<SolverPair>> GetIslands(const std::vector<SolverPair>& pairs);

        SolverRigidbody* FindRoot(SolverRigidbody* node);
        void AddPair(const SolverPair& p);
    };
}