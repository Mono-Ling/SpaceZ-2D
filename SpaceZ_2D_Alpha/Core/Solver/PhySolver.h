#pragma once
#include "Solver/SolverPair.h"
#include "Solver/SolverRigidbody.h"
#include "Object/Object.h"
#include <unordered_map>
#include <vector>

namespace Core::Solver
{
    class PhySolver
    {
    private:
        int _subStep;
        int _iteration;
        std::unordered_map<RigidbodyHandle, SolverRigidbody> _solverBodys;

    public:
        PhySolver(int subStep, int iteration)
        : _subStep(subStep), _iteration(iteration),
          _solverBodys(std::unordered_map<RigidbodyHandle, SolverRigidbody>()) {}

        void ImpulseIteration(std::vector<SolverPair>& island, float step) const;
    };
}
