using System;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    public static partial class PhySystem
    {
        /// <summary>
        /// 碰撞体底层封装：句柄登记、归属切换与材质/形状/变换下发
        /// <para>属性映射、Unity 生命周期与"相对归属刚体"的变换解析由具体组件
        /// <see cref="SpaceZ_2D.Beta.Collider2D_Z"/> 负责。</para>
        /// <para>与原生接口的差异：原生 CreateCollider 不带归属，归属靠随后的 SetColliderBody 挂接。</para>
        /// </summary>
        public abstract class Collider2D_Z : MonoBehaviour, INativeComponent
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
                    Debug.LogWarning($"【Beta 碰撞体】组件已激活，试图重复创建原生句柄{_handle}", this);
                    return;
                }
                _handle = PhySystem.CreateCollider(type);

                if(!_handle || TryGetCollider(_handle, out _, false))
                    throw new Exception("【Beta 碰撞体】创建原生碰撞体异常，句柄无效");

                if(_body)
                    PhySystem.SetColliderBody(_handle, _body);

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

                // 原生侧把"找不到的刚体"统一归入静态分支，托管侧按同一口径记账
                PhySystem.SetColliderBody(_handle, newBodyHandle);

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
                _dirtyCollidersProperty.Add(new ColliderProperty
                {
                    handle = _handle,
                    staticFriction = staticFriction,
                    dynamicFriction = dynamicFriction,
                    elasticity = elasticity,
                });
            }

            protected void SetColliderCollision(ulong layer, CollisionMode mode)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyCollidersCollision.Add(new ColliderCollision
                {
                    handle = _handle,
                    layer = layer,
                    mode = mode,
                });
            }

            /// <summary>下发碰撞体相对归属刚体的局部变换</summary>
            protected void SetColliderTransform(Vector2 pos, float rot, Vector2 scale)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyCollidersTransform.Add(new TransformInfo
                {
                    handle = _handle,
                    position = pos,
                    rotation = rot,
                    scale = scale,
                });
            }
            #endregion

            #region 形状
            /// <summary>线段全长（沿局部 X 轴，中心在原点）</summary>
            protected void SetLine(float length)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyLineColliders.Add(new LineLength
                {
                    handle = _handle,
                    length = length,
                });
            }

            /// <summary>盒半长（各轴半径）</summary>
            protected void SetBox(Vector2 extents)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyBoxColliders.Add(new BoxExtents
                {
                    handle = _handle,
                    extents = extents,
                });
            }

            /// <summary>球半径</summary>
            protected void SetSphere(float radius)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtySphereColliders.Add(new SphereRadius
                {
                    handle = _handle,
                    radius = radius,
                });
            }

            /// <summary>胶囊半径与圆柱段高度（不含两端半球）</summary>
            protected void SetCapsule(float radius, float height)
            {
                if(!TryGetCollider(_handle, out _, false))
                    return;
                _dirtyCapsuleColliders.Add(new CapsuleSize
                {
                    handle = _handle,
                    radius = radius,
                    height = height,
                });
            }
            #endregion
        }
    }
}
