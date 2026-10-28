#pragma once
#include "Solver/SolverRigidbody.h"
#include "Solver/SolverCollisionPoint.h"
#include "Collision/Info/CollisionInfo.h"
#include <utility>
#include <vector>

namespace Core::Solver
{
    /// @brief 求解对
    /// @brief 用于同一对碰撞体间多碰撞点解算
    struct SolverPair
    {
        SolverRigidbody* first = nullptr;
        SolverRigidbody* second = nullptr;

        float staticFriction = 0;
        float dynamicFriction = 0;
        float elasticity = 0;

        std::vector<SolverCollisionPoint> solverPoints;

        SolverPair() = default;
        explicit SolverPair(const std::vector<Collision::CollisionInfo>& infos);

        explicit operator bool() const;

        void SetBody(SolverRigidbody* first, SolverRigidbody* second);
        void SetContact(const std::pair<float, float>& sf, const std::pair<float, float>& df, const std::pair<float, float>& e);

        void InitSolverPair();
        void ApplyNormalImpulse();
        void ApplyTangentImpulse();
        void ApplyPositionCorrection();
    };
}