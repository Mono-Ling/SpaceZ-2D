#pragma once
#include "Solver/SolverRigidbody.h"
#include "Math/Vector2.h"
#include <utility>

namespace Core::Solver
{
    struct SolverCollisionPoint
    {
        float invNormalEffMass = 0;
        float invTangentEffMass = 0;

        float targetRelatVelocity = 0;
        mutable float normalImpulse = 0; // first -> second
        mutable float tangentImpulse = 0;

        Math::Vector2 point = Math::Vector2::zero;
        Math::Vector2 normal = Math::Vector2::zero; // first -> second

        // 刚体本地坐标系下（含缩放）碰撞点
        std::pair<Math::Vector2, Math::Vector2> anchorPoint;
        std::pair<Math::Vector2, Math::Vector2> radiusVector;

        SolverCollisionPoint(const Math::Vector2& p, const Math::Vector2& n, const std::pair<Math::Vector2, Math::Vector2>& anchor)
        : point(p), normal(n.Normalized()), anchorPoint(anchor) {}

        void SetBody(SolverRigidbody* const first, SolverRigidbody* const second);
        void SetTargetRelatVelocity(float elasticity, SolverRigidbody* const first, SolverRigidbody* const second);
        Math::Vector2 GetDeltaNormalImpulse(SolverRigidbody* const first, SolverRigidbody* const second) const;
        Math::Vector2 GetTangentImpulse(float sf, float df, SolverRigidbody* const first, SolverRigidbody* const second) const;
        Math::Vector2 GetPositionCorrection(SolverRigidbody* const first, SolverRigidbody* const second) const;
    
    private:
        float GetInvEffMass(const Math::Vector2& dir, SolverRigidbody* const first, SolverRigidbody* const second) const;
    };
}