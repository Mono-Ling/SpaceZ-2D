#include "UnityPlugin/ExportInterface/ExportInterface.h"
#include "UnityPlugin/ExportData/Handle.h"
#include "UnityPlugin/ExportData/ObjInfo.h"
#include "UnityPlugin/ExportData/ObjProperty.h"
using namespace Core;
using namespace Core::Math;

// 对象层导出：把 Core::PhySystem 的刚体/碰撞体接口映射成 C 函数。
// 与 PhyEngine.cpp 的分工：那边是引擎生命周期与步进结果（脏刚体、碰撞对），
// 这边只管对象的创建、销毁、属性与变换写入。
//
// 传参约定
//   1. C# -> C++ 一律裸指针：批量接口签名固定为 (ObjInfo<T, W>* infos, int count)，
//      由调用方 pin 住托管数组后把首地址传进来。**不要**用 NativeArray 包入参 ——
//      那是 C++ -> C# 出参方向的容器，套到入参上只会多一次布局转换。
//   2. C++ -> C# 只有 Create* 需要回句柄，返回 UnityPlugin::Handle（POD）。

namespace UnityPlugin
{
#pragma region 对象生命周期
    /// @brief 创建刚体
    /// @return 刚体句柄，失败返回 id == -1 的空句柄
    EXPORT_FUNCTION Handle CreateBody()
    {
        if(!PhyAvailable())
            return {};
        TRY_CATCH_DEBUG
        (
            { return Handle::From(g_phySystem->CreateBody()); },
            "PhyObject",
            "刚体创建异常"
        );
        return {};
    }
    /// @brief 创建碰撞体
    /// @param type 碰撞体形状（Line / Box / Sphere / Capsule）
    /// @return 碰撞体句柄，失败返回 id == -1 的空句柄
    /// @note 新建碰撞体默认没有归属刚体，即静态碰撞体，要挂到刚体上须再调 SetColliderBody
    EXPORT_FUNCTION Handle CreateCollider(ColliderType type)
    {
        if(!PhyAvailable())
            return {};
        TRY_CATCH_DEBUG
        (
            { return Handle::From(g_phySystem->CreateCollider(type)); },
            "PhyObject",
            "碰撞体创建异常"
        );
        return {};
    }
    /// @brief 销毁刚体，其名下所有碰撞体一并销毁
    /// @return 是否销毁成功；句柄不存在或已失效返回 false
    /// @note C# 侧返回值要按 1 字节封送（[return: MarshalAs(UnmanagedType.I1)]），默认 4 字节 BOOL 会读到高位垃圾
    EXPORT_FUNCTION bool DestroyBody(RigidbodyHandle handle)
    {
        if(!PhyAvailable())
            return false;
        TRY_CATCH_DEBUG
        (
            { return g_phySystem->DestroyBody(handle); },
            "PhyObject",
            "刚体销毁异常"
        );
        return false;
    }
    /// @brief 销毁碰撞体
    /// @return 是否销毁成功；句柄不存在或已失效返回 false
    /// @note C# 侧返回值要按 1 字节封送（[return: MarshalAs(UnmanagedType.I1)]）
    EXPORT_FUNCTION bool DestroyCollider(ColliderHandle handle)
    {
        if(!PhyAvailable())
            return false;
        TRY_CATCH_DEBUG
        (
            { return g_phySystem->DestroyCollider(handle); },
            "PhyObject",
            "碰撞体销毁异常"
        );
        return false;
    }
    /// @brief 设置碰撞体的归属刚体
    /// @param body 归属刚体；传空句柄（id == -1）即解除挂接，碰撞体退回静态碰撞体
    /// @note 碰撞体变换是相对归属刚体的局部变换，换父节点后世界姿态会随之改变
    EXPORT_FUNCTION void SetColliderBody(ColliderHandle collider, RigidbodyHandle body)
    {
        if(!PhyAvailable())
            return;
        TRY_CATCH_DEBUG
        (
            { g_phySystem->SetColliderBody(collider, body); },
            "PhyObject",
            "碰撞体归属刚体设置异常"
        );
    }
#pragma endregion

#pragma region 刚体批量写入
    /// @brief 批量设置刚体质量、惯量、重力与阻尼
    /// @param infos (handle, {mass, inertia, useGravity, linearDrag, angularDrag})
    /// @param count 元素个数
    /// @note mass/inertia 内部钳到不小于 Epsilon，阻尼负值钳到 0
    EXPORT_FUNCTION void SetBodiesProperty(ObjInfo<Rigidbody, BodyProperty>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->SetBodyProperty(curr.handle, curr.info.mass, curr.info.inertia, curr.info.useGravity,
                                                 curr.info.linearDrag, curr.info.angularDrag);
                }
            },
            "PhyObject",
            "刚体属性设置异常"
        );
    }
    /// @brief 批量覆写刚体速度（不累加，直接覆盖）
    /// @param infos (handle, {linear, angular})
    EXPORT_FUNCTION void SetBodiesVelocity(ObjInfo<Rigidbody, BodyVelocity>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->SetVelocity(curr.handle, curr.info.linear, curr.info.angular);
                }
            },
            "PhyObject",
            "刚体速度设置异常"
        );
    }
    /// @brief 批量给刚体加力（本帧累积，步进时消耗）
    /// @param infos (handle, {force, mode})，Impulse 为一次性冲量，Sustain 为持续力
    EXPORT_FUNCTION void AddForces(ObjInfo<Rigidbody, ForceInfo>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->AddForce(curr.handle, curr.info.force, curr.info.mode);
                }
            },
            "PhyObject",
            "刚体加力异常"
        );
    }
    /// @brief 批量给刚体加力矩（本帧累积，步进时消耗）
    /// @param infos (handle, {torque, mode})，Impulse 为一次性角冲量，Sustain 为持续力矩
    EXPORT_FUNCTION void AddTorques(ObjInfo<Rigidbody, TorqueInfo>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->AddTorque(curr.handle, curr.info.torque, curr.info.mode);
                }
            },
            "PhyObject",
            "刚体加力矩异常"
        );
    }
#pragma endregion

#pragma region 碰撞体批量写入
    /// @brief 批量设置碰撞体摩擦与弹性
    /// @param infos (handle, {staticFriction, dynamicFriction, elasticity})
    /// @note 系数负值钳到 0，恢复系数钳到 [0, 1]
    EXPORT_FUNCTION void SetCollidersProperty(ObjInfo<Collider, ColliderProperty>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->SetColliderProperty(curr.handle, curr.info.staticFriction, curr.info.dynamicFriction,
                                                     curr.info.elasticity);
                }
            },
            "PhyObject",
            "碰撞体属性设置异常"
        );
    }
    /// @brief 批量设置碰撞体所在层与碰撞模式
    /// @param infos (handle, {layer, mode})，layer 是 64 位层掩码
    /// @note Continuous 需碰撞体挂在刚体上才生效，否则退化为 Discrete
    EXPORT_FUNCTION void SetCollidersCollision(ObjInfo<Collider, ColliderCollision>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->SetColliderCollision(curr.handle, Collision::LayerMask(curr.info.layer), curr.info.mode);
                }
            },
            "PhyObject",
            "碰撞体碰撞设置异常"
        );
    }
    /// @brief 批量设置变换（刚体），同时把名下碰撞体标脏重建包围盒
    /// @param infos (handle, {position, rotation, scale})
    /// @note rotation 单位是弧度；scale 只有"大小"语义，须非负
    EXPORT_FUNCTION void SetBodiesTransform(ObjInfo<Rigidbody, Transform>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->SetTransform(curr.handle, curr.info.position, curr.info.rotation, curr.info.scale);
                }
            },
            "PhyObject",
            "刚体变换设置异常"
        );
    }
    /// @brief 批量设置变换（碰撞体），相对归属刚体；无归属时即世界变换
    /// @param infos (handle, {position, rotation, scale})
    EXPORT_FUNCTION void SetCollidersTransform(ObjInfo<Collider, Transform>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->SetTransform(curr.handle, curr.info.position, curr.info.rotation, curr.info.scale);
                }
            },
            "PhyObject",
            "碰撞体变换设置异常"
        );
    }
#pragma endregion

#pragma region 碰撞形状批量写入
    /// @brief 批量设置线段长度
    EXPORT_FUNCTION void SetLineColliders(ObjInfo<Collider, float>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                    g_phySystem->SetLineCollider(infos[i].handle, infos[i].info);
            },
            "PhyObject",
            "线段碰撞体设置异常"
        );
    }
    /// @brief 批量设置盒半长（各轴半径，不是全尺寸）
    EXPORT_FUNCTION void SetBoxColliders(ObjInfo<Collider, Vector2>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                    g_phySystem->SetBoxCollider(infos[i].handle, infos[i].info);
            },
            "PhyObject",
            "盒碰撞体设置异常"
        );
    }
    /// @brief 批量设置球半径
    EXPORT_FUNCTION void SetSphereColliders(ObjInfo<Collider, float>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                    g_phySystem->SetSphereCollider(infos[i].handle, infos[i].info);
            },
            "PhyObject",
            "球碰撞体设置异常"
        );
    }
    /// @brief 批量设置胶囊半径与圆柱段高度（不含两端半球）
    EXPORT_FUNCTION void SetCapsuleColliders(ObjInfo<Collider, CapsuleSize>* infos, int count)
    {
        if(!PhyAvailable() || !infos || count <= 0)
            return;
        TRY_CATCH_DEBUG
        (
            {
                for(int i = 0; i < count; i++)
                {
                    auto& curr = infos[i];
                    g_phySystem->SetCapsuleCollider(curr.handle, curr.info.radius, curr.info.height);
                }
            },
            "PhyObject",
            "胶囊碰撞体设置异常"
        );
    }
#pragma endregion
}
