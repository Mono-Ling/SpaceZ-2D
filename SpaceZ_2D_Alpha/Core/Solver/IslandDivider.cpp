#include "Solver/IslandDivider.h"
using namespace std;

namespace Core::Solver
{
    vector<vector<SolverPair>> IslandDivider::IslandDivision(const vector<SolverPair>& pairs)
    {
        SetSolverPairs(pairs);
        return GetIslands(pairs);
    }

    void IslandDivider::SetSolverPairs(const vector<SolverPair>& pairs)
    {
        _unionSet.clear();
        for(auto& p : pairs)
        {
            if(!p.IsEnable())
                continue;
            if(_unionSet.find(p.first) == _unionSet.end())
                _unionSet.insert({p.first, p.first});

            if(_unionSet.find(p.second) == _unionSet.end())
                _unionSet.insert({p.second, p.second});
        }

        for(auto& p : pairs)
            AddPair(p);
    }
    vector<vector<SolverPair>> IslandDivider::GetIslands(const vector<SolverPair>& pairs)
    {
        unordered_map<SolverRigidbody*, vector<SolverPair>> islandMap;
        for(auto& p : pairs)
        {
            if(!p.IsEnable())
                continue;
            auto root = FindRoot(p.first);
            if(root != FindRoot(p.second))
                continue;
            islandMap[root].push_back(p);
        }
        vector<vector<SolverPair>> ans;
        for(auto& p : islandMap)
            ans.push_back(p.second);
        return ans;
    }

    SolverRigidbody* IslandDivider::FindRoot(SolverRigidbody* node)
    {
        auto curr = node;
        while (_unionSet[curr] != curr)
            curr = _unionSet[curr];

        _unionSet[node] = curr;
        return curr;
    }
    void IslandDivider::AddPair(const SolverPair& p)
    {
        if(!p.IsEnable())
            return;
        auto a = FindRoot(p.first);
        auto b = FindRoot(p.second);
        if(a == b)
            return;
        _unionSet[a] = b;
    }
}