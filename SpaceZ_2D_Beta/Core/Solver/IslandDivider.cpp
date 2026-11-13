#include "Core/Solver/IslandDivider.h"
#include "Core/Debug/DebugFormat.h"
#include <utility>
using namespace std;

namespace Core::Solver
{
    vector<vector<SolverPair>> IslandDivider::Islands(vector<IslandDivisionReq>& reqs)
    {
        _unionFind.Clear();
        _islandMap.clear();

        for(auto& req : reqs)
            _unionFind.AddPair(req.first, req.second);

        for(auto& req : reqs)
        {
            auto rootA = _unionFind.FindRoot(req.first);
            auto rootB = _unionFind.FindRoot(req.second);

            if(rootA != rootB)
            {
                Debug::LogWarningFormat("【解算岛屿划分器】出现未连通解算对", req.first, req.second);
                continue;
            }
            
            _islandMap[rootA].emplace_back(move(req.solverPair));
        }

        vector<vector<SolverPair>> ans;
        ans.reserve(_islandMap.size());
        for(auto& island : _islandMap)
            ans.emplace_back(move(island.second));

        return ans;
    }
}