#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Math/Vector2.h"
#include "Core/System/Info/RaycastHit.h"

namespace UnityPlugin
{
    /// @brief 射线命中结果
    /// @note 布局：collider@0（8 字节）point@8 normal@16 distance@24，共 28 字节。
    ///       distance 为 -1 表示未命中（与 Core::RayHit 的判定一致）。
    struct RaycastHit
    {
        Core::ColliderHandle collider;
        Core::Math::Vector2 point;
        Core::Math::Vector2 normal;
        float distance = -1.0f;

        RaycastHit() = default;
        RaycastHit(const Core::RaycastHit& core)
        : collider(core.collider), point(core.point), normal(core.normal), distance(core.distance) {}
    };
}
