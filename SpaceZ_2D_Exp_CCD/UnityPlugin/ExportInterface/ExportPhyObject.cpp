#include "ExportInterface/ExportInterface.h"
#include "ExportInterface/ExportData/ExportHandle.h"
#include "ExportInterface/ExportData/ExportObjInfo.h"
#include "ExportInterface/ExportData/BodyProperty.h"
#include "ExportInterface/ExportData/ColliderProperty.h"
#include "ExportInterface/ExportData/ExportTransform.h"
#include "Math/Vector2.h"
#include <utility>
using namespace Core;
using namespace Core::Math;
using namespace ExportData;
using namespace std;

#pragma region 对象生命周期
EXPORT_FUNCTION ExportHandle CreateBody()
{
    if(!IsEngineAvailable())
        return ExportHandle::From(RigidbodyHandle::null);
    TRY_CATCH_DEBUG
    (
        { ExportHandle::From(phySystem->CreateBody()); },
        "物理对象",
        "刚体创建异常"
    );
    return ExportHandle::From(RigidbodyHandle::null);
}
EXPORT_FUNCTION ExportHandle CreateCollider(RigidbodyHandle body, ColliderType type)
{
    if(!IsEngineAvailable())
        return ExportHandle::From(ColliderHandle::null);
    TRY_CATCH_DEBUG
    (
        { ExportHandle::From(phySystem->CreateCollider(body, type)); },
        "物理对象",
        "碰撞体创建异常"
    );
    return ExportHandle::From(ColliderHandle::null);
}
EXPORT_FUNCTION void DestroyBody(RigidbodyHandle handle)
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        { phySystem->DestroyBody(handle); },
        "物理对象",
        "刚体销毁异常"
    );
}
EXPORT_FUNCTION void DestroyCollider(ColliderHandle handle)
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        { phySystem->DestroyCollider(handle); },
        "物理对象",
        "碰撞体销毁异常"
    );
}
EXPORT_FUNCTION void SetColliderRigidbody(ColliderHandle collider, RigidbodyHandle body)
{
    if(!IsEngineAvailable())
        return;
    TRY_CATCH_DEBUG
    (
        { phySystem->SetColliderRigidbody(collider, body); },
        "物理对象",
        "碰撞体归属刚体设置异常"
    );
}
#pragma endregion

#pragma region 批量数据同步
EXPORT_FUNCTION void SetBodiesProperty(ExportObjInfo<Rigidbody, BodyProperty>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
            {
                auto& curr = infos[i];
                phySystem->SetBody(curr.handle, curr.info.mass, curr.info.inertia, curr.info.useGravity);
            }
        },
        "物理对象",
        "刚体属性设置异常"
    );
}
EXPORT_FUNCTION void SetBodiesDrag(ExportObjInfo<Rigidbody, pair<float, float>>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
            {
                auto& curr = infos[i];
                phySystem->SetBodyDrag(curr.handle, curr.info.first, curr.info.second);
            }
        },
        "物理对象",
        "刚体阻尼设置异常"
    );
}
EXPORT_FUNCTION void SetBodiesVelocity(ExportObjInfo<Rigidbody, pair<Vector2, float>>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
            {
                auto& curr = infos[i];
                phySystem->SetVelocity(curr.handle, curr.info.first, curr.info.second);
            }
        },
        "物理对象",
        "刚体速度设置异常"
    );
}
EXPORT_FUNCTION void SetCollidersProperty(ExportObjInfo<Collider, ColliderProperty>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
            {
                auto& curr = infos[i];
                phySystem->SetCollider(curr.handle, curr.info.staticFriction, curr.info.dynamicFriction, curr.info.elasticity);
            }
        },
        "物理对象",
        "碰撞体属性设置异常"
    );
}
EXPORT_FUNCTION void SetBodiesTransform(ExportObjInfo<Rigidbody, ExportTransform>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
            {
                auto& curr = infos[i];
                phySystem->UpdateTransform(curr.handle, curr.info.position, curr.info.rotation, curr.info.scale);
            }
        },
        "物理对象",
        "刚体变换设置异常"
    );
}
EXPORT_FUNCTION void SetCollidersTransform(ExportObjInfo<Collider, ExportTransform>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
            {
                auto& curr = infos[i];
                phySystem->UpdateTransform(curr.handle, curr.info.position, curr.info.rotation, curr.info.scale);
            }
        },
        "物理对象",
        "碰撞体变换设置异常"
    );
}
EXPORT_FUNCTION void SetBoxColliders(ExportObjInfo<Collider, Vector2>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystem->SetBoxCollider(infos[i].handle, infos[i].info);
        },
        "物理对象",
        "碰撞盒设置异常"
    );
}
EXPORT_FUNCTION void SetSphereColliders(ExportObjInfo<Collider, float>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystem->SetSphereCollider(infos[i].handle, infos[i].info);
        },
        "物理对象",
        "碰撞球体设置异常"
    );
}
EXPORT_FUNCTION void SetCapsuleColliders(ExportObjInfo<Collider, pair<float, float>>* infos, int count)
{
    if(!IsEngineAvailable() || !infos || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystem->SetCapsuleCollider(infos[i].handle, infos[i].info.first, infos[i].info.second);
        },
        "物理对象",
        "碰撞胶囊体设置异常"
    );
}
#pragma endregion

#pragma region 刚体加力
EXPORT_FUNCTION void AddForces(ExportObjInfo<Rigidbody, pair<Vector2, ForceType>>* forces, int count)
{
    if(!IsEngineAvailable() || !forces || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystem->AddForce(forces[i].handle, forces[i].info.first, forces[i].info.second);
        },
        "物理对象",
        "刚体加力异常"
    );
}
EXPORT_FUNCTION void AddTorques(ExportObjInfo<Rigidbody, pair<float, ForceType>>* torques, int count)
{
    if(!IsEngineAvailable() || !torques || count <= 0)
        return;
    TRY_CATCH_DEBUG
    (
        {
            for(int i = 0; i < count; i++)
                phySystem->AddTorque(torques[i].handle, torques[i].info.first, torques[i].info.second);
        },
        "物理对象",
        "刚体加力矩异常"
    );
}
#pragma endregion