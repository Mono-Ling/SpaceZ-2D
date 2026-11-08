using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    public static partial class PhySystem
    {
        /// <summary>
        /// 碰撞体底层封装：句柄登记、归属切换与材质/形状/变换下发。
        /// <para>属性映射、Unity 生命周期与"相对刚体"的变换解析由派生类
        /// <see cref="CCD.Collider2D_Exp_CCD"/> 负责。</para>
        /// </summary>
        public abstract class Collider2D_Exp_CCD : MonoBehaviour, INativeComponent
        {
            public Handle Handle => _handle;
            /// <summary>归属刚体句柄，静态碰撞体为 Handle.NULL</summary>
            public Handle BodyHandle => _body;
            protected Handle _handle = Handle.NULL;
            protected Handle _body = Handle.NULL;

            public abstract void ComponentUpdate();

            #region 物理对象生命周期
            /// <summary>创建原生碰撞体并登记到托管表（归属句柄须先写入 _body）</summary>
            protected void CreateCollider(ColliderType type)
            {
                if(TryGetCollider(_handle, out _, false))
                {
                    Debug.LogWarning($"【EXP CCD碰撞体】碰撞体组件已激活，试图重复创建原生句柄{_handle}", this);
                    return;
                }
                _handle = PhySystem.CreateCollider(_body, type);

                if(!_handle || TryGetCollider(_handle, out _, false))
                    throw new Exception("【EXP CCD碰撞体】创建原生碰撞体异常，句柄无效");

                _colliders.Add(_handle, this);
                _nativeComponents.Add(this);

                if(TryGetBody(_body, out _, false) && _dynamicColliders.TryGetValue(_body, out var set))
                    set?.Add(_handle);
            }
            /// <summary>销毁原生碰撞体并摘除托管登记</summary>
            /// <param name="native">false 表示原生侧已随刚体一起销毁</param>
            /// <param name="dynamicColliderSet">false 表示刚体正在销毁，动态集合无需维护</param>
            public void DestroyCollider(bool native = true, bool dynamicColliderSet = true)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                if(native)
                    PhySystem.DestroyCollider(_handle);

                _colliders.Remove(_handle);
                _nativeComponents.Remove(this);

                if(dynamicColliderSet && _dynamicColliders.TryGetValue(_body, out var set))
                    set?.Remove(_handle);

                _handle = Handle.NULL;
                _body = Handle.NULL;
            }
            /// <summary>切换归属刚体，Handle.NULL 表示转为静态碰撞体</summary>
            protected void SetBody(Handle newBodyHandle)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                if(_body == newBodyHandle)
                    return;

                // 原生侧会把"找不到的刚体"统一归入静态分支，托管侧按同一口径记账
                PhySystem.SetColliderRigidbody(_handle, newBodyHandle);

                if(_dynamicColliders.TryGetValue(_body, out var oldSet))
                    oldSet?.Remove(_handle);

                _body = newBodyHandle;

                if(_dynamicColliders.TryGetValue(_body, out var newSet))
                    newSet?.Add(_handle);
            }
            #endregion

            #region 数据下发
            protected void SetColliderProperty(float staticFriction, float dynamicFriction, float elasticity)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyCollidersProperty.Add(new()
                {
                    handle = _handle,
                    staticFriction = staticFriction,
                    dynamicFriction = dynamicFriction,
                    elasticity = elasticity
                });
            }
            /// <summary>下发碰撞体相对归属刚体的局部变换</summary>
            protected void SetColliderTransform(Vector2 pos, float rot, Vector2 scale)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyCollidersTransform.Add(new()
                {
                    handle = _handle,
                    position = pos,
                    rotation = rot,
                    scale = scale
                });
            }
            #endregion

            #region 形状
            /// <summary>矩形半长，仅 Box 碰撞体有效</summary>
            protected void SetBox(Vector2 extents)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyBoxColliders.Add(new()
                {
                    handle = _handle,
                    extents = extents
                });
            }
            /// <summary>圆半径，仅 Sphere 碰撞体有效</summary>
            protected void SetSphere(float radius)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtySphereColliders.Add(new()
                {
                    handle = _handle,
                    radius = radius
                });
            }
            /// <summary>胶囊半径与两端球心间距（不含半球），仅 Capsule 碰撞体有效</summary>
            protected void SetCapsule(float radius, float height)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyCapsuleColliders.Add(new()
                {
                    handle = _handle,
                    radius = radius,
                    height = height
                });
            }
            #endregion
        }
    }
}
