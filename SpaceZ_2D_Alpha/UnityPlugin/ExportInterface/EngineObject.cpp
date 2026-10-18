#include "ExportInterface/ExportInterface.h"
#include "System/PhySystem.h"
#include "ExportInterface/ExportInfo/ExportHandle.h"
#include "ExportInterface/ExportInfo/ObjectInfo.h"
#include "ExportInterface/ExportInfo/TransformInfo.h"
#include "ExportInterface/ExportInfo/ColliderInfo.h"
#include <utility>
using namespace Core;
using namespace Core::Math;
using namespace Export;
using namespace std;

#pragma region 生命周期
EXPORT_FUNCTION ExportHandle<Rigidbody> CreateBody()
{
    if(!IsEngineAvailable())
        return ExportHandle<Rigidbody>::From(RigidbodyHandle::null);
    TRY_CATCH_DEBUG
    (
        { return ExportHandle<Rigidbody>::From(phySystemPtr->CreateBody()); },
        "物理对象",
        "刚体创建异常"
    );
    return ExportHandle<Rigidbody>::From(RigidbodyHandle::null);
}
EXPORT_FUNCTION ExportHandle<Collider> CreateCollider(ColliderType type, RigidbodyHandle body)
{
    if(!IsEngineAvailable())
        return ExportHandle<Collider>::From(ColliderHandle::null);
    TRY_CATCH_DEBUG
    (
        { return ExportHandle<Collider>::From(phySystemPtr->CreateCollider(type, body)); },
        "物理对象",
        "碰撞体创建异常"
    );
    return ExportHandle<Collider>::From(ColliderHandle::null);
}
EXPORT_FUNCTION void DestroyBody(RigidbodyHandle handle)
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        { phySystemPtr->DestroyBody(handle); },
        "物理对象",
        "销毁刚体异常"
    );
}
EXPORT_FUNCTION void DestroyCollider(ColliderHandle handle)
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        { phySystemPtr->DestroyCollider(handle); },
        "物理对象",
        "销毁碰撞体异常"
    );
}
EXPORT_FUNCTION void SetColliderBody(ObjectInfo<Collider, RigidbodyHandle> info)
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        {
            phySystemPtr->SetColliderRigidbody(info.handle, info.info);
            phySystemPtr->SynBreadthPhase();
        },
        "物理对象",
        "设置碰撞体所属刚体"
    );
}
#pragma endregion

#pragma region 数据批量同步
/// @brief 设置刚体信息
/// @param infos 刚体信息数组(handle, mass, inertia)
/// @param count 数组长度
/// @return 
EXPORT_FUNCTION void SetBodies(ObjectInfo<Rigidbody, pair<float, float>>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->SetBody(infos[i].handle, infos[i].info.first, infos[i].info.second);
        },
        "物理对象",
        "刚体信息设置异常"
    );
}
/// @brief 设置刚体速度
/// @param infos 刚体速度数组(handle, linearV, angularV)
/// @param count 数组长度
/// @return 
EXPORT_FUNCTION void SetBodiesVelocity(ObjectInfo<Rigidbody, pair<Vector2, float>>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->SetVelocity(infos[i].handle, infos[i].info.first, infos[i].info.second);
        },
        "物理对象",
        "刚体速度设置异常"
    );
}
EXPORT_FUNCTION void SetColliders(ObjectInfo<Collider, ColliderInfo>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->SetCollider(infos[i].handle, infos[i].info.staticFriction, infos[i].info.dynamicFriction, infos[i].info.elasticity);
        },
        "物理对象",
        "碰撞体设置异常"
    );
}
EXPORT_FUNCTION void SetBodiesTransform(ObjectInfo<Rigidbody, TransformInfo>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->UpdateTransform(infos[i].handle, infos[i].info.position, infos[i].info.rotation, infos[i].info.scale);
            phySystemPtr->SynBreadthPhase();
        },
        "物理对象",
        "刚体变换设置异常"
    );
}
EXPORT_FUNCTION void SetCollidersTransform(ObjectInfo<Collider, TransformInfo>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->UpdateTransform(infos[i].handle, infos[i].info.position, infos[i].info.rotation, infos[i].info.scale);
            phySystemPtr->SynBreadthPhase();
        },
        "物理对象",
        "碰撞体变换设置异常"
    );
}
EXPORT_FUNCTION void SetBoxColliders(ObjectInfo<Collider, Vector2>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->SetBoxCollider(infos[i].handle, infos[i].info);
            phySystemPtr->SynBreadthPhase();
        },
        "物理对象",
        "矩形碰撞体设置异常"
    );
}
EXPORT_FUNCTION void SetSphereColliders(ObjectInfo<Collider, float>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->SetSphereCollider(infos[i].handle, infos[i].info);
            phySystemPtr->SynBreadthPhase();
        },
        "物理对象",
        "圆形碰撞体设置异常"
    );
}
/// @brief 设置胶囊碰撞体
/// @param infos 胶囊碰撞体信息数组(handle, height, radius)
/// @param count 数组长度
/// @return 
EXPORT_FUNCTION void SetCapsuleColliders(ObjectInfo<Collider, pair<float, float>>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->SetCapsuleCollider(infos[i].handle, infos[i].info.first, infos[i].info.second);
            phySystemPtr->SynBreadthPhase();
        },
        "物理对象",
        "胶囊碰撞体设置异常"
    );
}
#pragma endregion

#pragma region 刚体加力
EXPORT_FUNCTION void AddForce(ObjectInfo<Rigidbody, Vector2>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->AddForce(infos[i].handle, infos[i].info);
        },
        "物理对象",
        "刚体施加线性力异常"
    );
}
EXPORT_FUNCTION void AddTorque(ObjectInfo<Rigidbody, float>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystemPtr->AddTorque(infos[i].handle, infos[i].info);
        },
        "物理对象",
        "刚体施加力矩异常"
    );
}
#pragma endregion