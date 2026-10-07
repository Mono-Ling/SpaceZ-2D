#include "Solver/PhySolver.h"
using namespace std;

namespace Core::Solver
{
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
    }
}