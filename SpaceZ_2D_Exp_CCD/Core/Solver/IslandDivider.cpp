#include "Solver/IslandDivider.h"
#include "Debug/Debug.h"
using namespace std;

namespace Core::Solver
{
    vector<vector<SolverPair>> IslandDivider::GetIslands(const vector<SolverPair>& pairs)
    {
        InitUnionFind(pairs);

        unordered_map<SolverRigidbody*, vector<SolverPair>> islandMap;
        for(auto& p : pairs)
        {
            if(!p)
                continue;
            auto root = IsSameRoot(p.first, p.second);

            if(!root)
            {
                Debug::LogWarning("【岛屿划分器】岛屿构造出现未连通解算对");
                continue;
            }
            auto it = islandMap.find(root);
            if(it != islandMap.end())
                it->second.push_back(p);
            else
                islandMap.insert({root, {p}});
        }
        
        vector<vector<SolverPair>> ans;
        for(auto& island : islandMap)
            ans.push_back(island.second);
        return ans;
    }

    void IslandDivider::InitUnionFind(const std::vector<SolverPair>& pairs)
    {
        _unionFind.clear();
        for(auto& p : pairs)
            AddPair(p);
    }
    SolverRigidbody* const IslandDivider::FindRoot(SolverRigidbody* const ptr)
    {
        if(!ptr || !_unionFind.count(ptr))
            return nullptr;
        auto curr = ptr;
        while (_unionFind[curr] != curr)
            curr = _unionFind[curr];
        _unionFind[ptr] = curr;
        return curr;
    }
    void IslandDivider::AddPair(const SolverPair& p)
    {
        if(!p)
            return;
        
        auto rootA = FindRoot(p.first);
        auto rootB = FindRoot(p.second);

        if(!rootA)
        {
            rootA = p.first;
            _unionFind[rootA] = rootA;
        }
        if(!rootB)
        {
            rootB = p.second;
            _unionFind[rootB] = rootB;
        }

        if(rootA == rootB)
            return;
        _unionFind[rootA] = rootB;
    }
    SolverRigidbody* const IslandDivider::IsSameRoot(SolverRigidbody* a, SolverRigidbody* b)
    {
        if(!a || !b)
            return nullptr;
        auto rootA = FindRoot(a);
        auto rootB = FindRoot(b);
        if(!rootA || !rootB)
        {
            Debug::LogWarning("【岛屿划分器】寻根失败，节点不存在于并查集中");
            return nullptr;
        }
        return rootA == rootB ? rootA : nullptr;
    }
}