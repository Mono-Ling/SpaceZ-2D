#pragma once
#include "Core/Solver/SolverRigidbody.h"
#include "Core/Solver/SolverCollisionPoint.h"
#include "Core/Collision/Info/CollisionInfo.h"
#include <utility>
#include <vector>

namespace Core::Solver
{
    struct SolverPair
    {
        SolverRigidbody* first = nullptr;
        SolverRigidbody* second = nullptr;

        float staticFriction = 0;
        float dynamicFriction = 0;
        float elasticity = 0;

    private:
        std::vector<SolverCollisionPoint> _points;
        std::vector<Math::Vector2> _deltaImpulseList;

    public:
        SolverPair(const std::vector<Collision::CollisionInfo>& infos, SolverRigidbody* f, SolverRigidbody* s);

        explicit operator bool() const;

        void SetContact(const std::pair<float, float>& sf, const std::pair<float, float>& df, const std::pair<float, float>& e);
        void Precompute();

        void ApplyNormalImpulse();
        void ApplyTangentImpulse();
        void ApplyPositionCorrection();
    };
}