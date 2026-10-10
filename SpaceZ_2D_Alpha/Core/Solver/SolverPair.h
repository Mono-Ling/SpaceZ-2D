#pragma once
#include "Math/Vector2.h"
#include "Object/Collider.h"
#include "Solver/SolverRigidbody.h"
#include <utility>

namespace Core::Solver
{
    struct SolverPair
    {
        SolverRigidbody* first = nullptr;
        SolverRigidbody* second = nullptr;
        
        float staticFriction = 0;
        float dynamicFriction = 0;
        float elasticity = 0;

        float invNormalEffMass = 0;
        float invTangentEffMass = 0;
        float normalImpulse = 0; // first -> second
        float tangentImpulse = 0;

        Math::Vector2 point = Math::Vector2::zero;
        Math::Vector2 normal = Math::Vector2::zero; // first -> second

        // 刚体本地坐标系下（含缩放）碰撞点
        Math::Vector2 firstAnchorPoint = Math::Vector2::zero;
        Math::Vector2 secondAnchorPoint = Math::Vector2::zero;

        SolverPair(const Math::Vector2& point, const Math::Vector2& normal, std::pair<Math::Vector2, Math::Vector2> anchorPoint)
        : point(point), normal(normal.Normalized()),
          firstAnchorPoint(anchorPoint.first), secondAnchorPoint(anchorPoint.second) {}

        bool IsEnable() const;
        void SetRigidbody(SolverRigidbody* first, SolverRigidbody* second);
        void SetContact(const Collider& first, const Collider& second);

        void ApplyNormalImpulse();
        void ApplyTangentImpulse(float step);

        void ApplyPositionCorrection();
    
    private:
        float GetInvEffMass(const Math::Vector2& dir) const;
    };
}