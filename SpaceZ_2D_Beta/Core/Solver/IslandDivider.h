#pragma once
#include "Core/Solver/SolverRigidbody.h"
#include "Core/Solver/SolverPair.h"
#include "Core/Object/Container/UnionFindSet.h"
#include <unordered_map>
#include <vector>
#include <utility>

namespace Core::Solver
{
    struct IslandDivisionReq
    {
        SolverBodyHandle first;
        SolverBodyHandle second;
        SolverPair solverPair;
        
        IslandDivisionReq(const SolverBodyHandle& first, const SolverBodyHandle& second, SolverPair& solverPair)
        : first(first), second(second), solverPair(std::move(solverPair)) {}
    };
    
    class IslandDivider
    {
        Container::UnionFindSet<SolverRigidbody> _unionFind;
        std::unordered_map<SolverBodyHandle, std::vector<SolverPair>> _islandMap;

    public:
        std::vector<std::vector<SolverPair>> Islands(std::vector<IslandDivisionReq>& reqs);
    };
}