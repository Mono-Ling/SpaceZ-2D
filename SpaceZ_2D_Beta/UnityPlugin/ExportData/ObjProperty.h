#pragma once
#include "Core/Math/Vector2.h"
#include "Core/Object/PhyObject/Collider/CollisionMode.h"
#include "Core/Object/PhyObject/Rigidbody.h"

// 批量写入接口的载荷结构（仅入参方向，配合 UnityPlugin::ExportObjInfo 使用）。
// 全部标准布局、可平凡拷贝，C# 侧逐字段展平成 LayoutKind.Sequential 的 struct 即可逐字节对齐。
// 字段顺序与 Core::PhySystem 的形参顺序一致，不要为了"紧凑"重排 —— 一重排 C# 侧就跟不上。
// 这些结构只作入参载荷，不跨返回值方向，所以不追求严格 POD。

namespace UnityPlugin
{
#pragma region 刚体
    /// @brief 刚体物理属性，对应 PhySystem::SetBodyProperty
    /// @note 布局：mass@0 inertia@4 useGravity@8(1 字节 + 3 字节填充) linearDrag@12 angularDrag@16，共 20 字节
    ///       C# 侧 useGravity 必须标 [MarshalAs(UnmanagedType.U1)]，默认封送是 4 字节 BOOL，会把后面两个 float 顶偏
    struct BodyProperty
    {
        float mass = 1.0f;
        float inertia = 1.0f;
        bool useGravity = true;
        float linearDrag = 0.0f;
        float angularDrag = 0.05f;
    };
    /// @brief 刚体速度，对应 PhySystem::SetVelocity
    struct BodyVelocity
    {
        Core::Math::Vector2 linear;
        float angular = 0.0f;
    };
    /// @brief 刚体加力，对应 PhySystem::AddForce
    struct ForceInfo
    {
        Core::Math::Vector2 force;
        Core::ForceMode mode = Core::ForceMode::Impulse;
    };
    /// @brief 刚体力矩，对应 PhySystem::AddTorque
    struct TorqueInfo
    {
        float torque = 0.0f;
        Core::ForceMode mode = Core::ForceMode::Impulse;
    };
#pragma endregion

#pragma region 碰撞体
    /// @brief 碰撞体摩擦与弹性，对应 PhySystem::SetColliderProperty
    struct ColliderProperty
    {
        float staticFriction = 0.0f;
        float dynamicFriction = 0.0f;
        float elasticity = 0.0f;
    };
    /// @brief 碰撞体层与碰撞模式，对应 PhySystem::SetColliderCollision
    /// @note layer 直接传 64 位掩码而不是 LayerMask 结构体，避免跨语言的聚合体传参约定差异
    struct ColliderCollision
    {
        unsigned long long layer = 1ULL;
        Core::CollisionMode mode = Core::CollisionMode::Discrete;
    };
    /// @brief 胶囊尺寸，对应 PhySystem::SetCapsuleCollider
    /// @note height 是圆柱段高度，不含两端半球
    struct CapsuleSize
    {
        float radius = 0.5f;
        float height = 1.0f;
    };
#pragma endregion

#pragma region 变换
    /// @brief 局部变换，对应 PhySystem::SetTransform 的两个重载
    /// @note scale 在物理里只有"大小"语义，须非负
    struct Transform
    {
        Core::Math::Vector2 position;
        float rotation = 0.0f;
        Core::Math::Vector2 scale = Core::Math::Vector2::one;
    };
#pragma endregion
}
