#pragma once
#include "Core/Solver/SolverRigidbody.h"
#include "Core/Collision/Info/CollisionInfo.h"
#include "Core/Math/Vector2.h"
#include <utility>

namespace Core::Solver
{
    struct SolverCollisionPoint
    {
        float invNormalEffMass = 0;
        float invTangentEffMass = 0;

        float targetRelatVelocity = 0;
        mutable float normalImpulse = 0.0f;
        mutable float tangentImpulse = 0.0f;

        Math::Vector2 point = Math::Vector2::zero;
        Math::Vector2 normal = Math::Vector2::zero; // first -> second

        // 刚体本地坐标系下（含缩放）碰撞点
        std::pair<Math::Vector2, Math::Vector2> anchorPoint;
        std::pair<Math::Vector2, Math::Vector2> radiusVector;

        SolverCollisionPoint(const Collision::CollisionInfo& info, const SolverRigidbody* first, const SolverRigidbody* second);

        void SetTargetRelatVelocity(float elasticity, const SolverRigidbody* first, const SolverRigidbody* second);
        Math::Vector2 GetDeltaNormalImpulse(const SolverRigidbody* first, const SolverRigidbody* second) const;
        Math::Vector2 GetDeltaTangentImpulse(float sf, float df, const SolverRigidbody* first, const SolverRigidbody* second) const;
        Math::Vector2 GetPositionCorrection(const SolverRigidbody* first, const SolverRigidbody* second) const;
    
    private:
        float GetInvEffMass(const Math::Vector2& dir, const SolverRigidbody* first, const SolverRigidbody* second) const;
    };
}