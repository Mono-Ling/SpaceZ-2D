using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    public static partial class PhySystem
    {
        /// <summary>
        /// 刚体底层封装：句柄登记、销毁、批量数据下发与原生侧回写。
        /// <para>属性映射、Unity 生命周期与变换比较由派生类 <see cref="CCD.Rigidbody2D_Exp_CCD"/> 负责。</para>
        /// </summary>
        public abstract class Rigidbody2D_Exp_CCD : MonoBehaviour, INativeComponent
        {
            public Handle Handle => _handle;
            protected Handle _handle = Handle.NULL;

            /// <summary>原生回写的速度缓存</summary>
            protected Vector2 _linearVelocity;
            protected float _angularVelocity = 0.0f;
            /// <summary>回写时记录的 Unity 变换，供派生类比较以抑制回声下发</summary>
            protected (Vector2 pos, float rot, Vector2 scale) _preTransform;
            protected bool _transformSynced;

            public abstract void ComponentUpdate();

            #region 物理对象生命周期
            /// <summary>创建原生刚体并登记到托管表</summary>
            protected void CreateBody()
            {
                if(TryGetBody(_handle, out _, false))
                {
                    Debug.LogWarning($"【EXP CCD刚体】刚体组件已激活，试图重复创建原生句柄{_handle}", this);
                    return;
                }
                _handle = PhySystem.CreateBody();

                if(!_handle || TryGetBody(_handle, out _, false))
                    throw new Exception("【EXP CCD刚体】创建原生刚体异常，句柄无效");

                _bodies.Add(_handle, this);
                _dynamicColliders.Add(_handle, new());
                _nativeComponents.Add(this);
            }
            /// <summary>销毁原生刚体及其名下碰撞体，并摘除托管登记</summary>
            protected void DestroyBody()
            {
                if(!TryGetBody(_handle, out _, false))
                    return;

                if(_dynamicColliders.TryGetValue(_handle, out var set))
                {
                    foreach(var colliderHandle in set)
                        if(TryGetCollider(colliderHandle, out var collider, false))
                            collider?.DestroyCollider(false, false);
                    _dynamicColliders.Remove(_handle);
                }

                PhySystem.DestroyBody(_handle);
                _bodies.Remove(_handle);
                _nativeComponents.Remove(this);

                _handle = Handle.NULL;
                _linearVelocity = Vector2.zero;
                _angularVelocity = 0.0f;
            }
            #endregion

            #region 数据下发
            protected void SetBodyProperty(float mass, float inertia, bool useGravity)
            {
                if(!TryGetBody(_handle, out _, false))
                    return;
                _dirtyBodiesProperty.Add(new()
                {
                    handle = _handle,
                    mass = mass,
                    inertia = inertia,
                    useGravity = useGravity
                });
            }
            protected void SetBodyDrag(float linearDrag, float angularDrag)
            {
                if(!TryGetBody(_handle, out _, false))
                    return;
                _dirtyBodiesDrag.Add(new()
                {
                    handle = _handle,
                    linearDrag = linearDrag,
                    angularDrag = angularDrag
                });
            }
            protected void SetVelocity(Vector2 linearVelocity, float angularVelocity)
            {
                _linearVelocity = linearVelocity;
                _angularVelocity = angularVelocity;

                if(!TryGetBody(_handle, out _, false))
                    return;
                _dirtyBodiesVelocity.Add(new()
                {
                    handle = _handle,
                    linearVelocity = linearVelocity,
                    angularVelocity = angularVelocity
                });
            }
            protected void SetBodyTransform(Vector2 pos, float rot, Vector2 scale)
            {
                if(!TryGetBody(_handle, out _, false))
                    return;
                _dirtyBodiesTransform.Add(new()
                {
                    handle = _handle,
                    position = pos,
                    rotation = rot,
                    scale = scale
                });
            }
            #endregion

            #region 加力
            /// <summary>施加线性力</summary>
            /// <param name="type">冲量立即改变速度；持续力在本次步进内生效</param>
            public void AddForce(Vector2 force, ForceType type = ForceType.Impulse)
            {
                if(!TryGetBody(_handle, out _, false))
                    return;
                _bodyForces.Add(new()
                {
                    handle = _handle,
                    force = force,
                    type = type
                });
            }
            /// <summary>施加力矩</summary>
            /// <param name="type">冲量立即改变角速度；持续力矩在本次步进内生效</param>
            public void AddTorque(float torque, ForceType type = ForceType.Impulse)
            {
                if(!TryGetBody(_handle, out _, false))
                    return;
                _bodyTorques.Add(new()
                {
                    handle = _handle,
                    torque = torque,
                    type = type
                });
            }
            #endregion

            /// <summary>由原生侧回写位置与速度（仅帧循环调用）</summary>
            internal void SetBodyInfo(Vector2 pos, float rot, Vector2 linearVelocity, float angularVelocity)
            {
                if(!TryGetBody(_handle, out _, false))
                    return;

                transform.position = new Vector3(pos.x, pos.y, transform.position.z);
                var euler = transform.eulerAngles;
                transform.eulerAngles = new Vector3(euler.x, euler.y, rot * Mathf.Rad2Deg);

                _linearVelocity = linearVelocity;
                _angularVelocity = angularVelocity;

                // 记录 Unity 归一化后的角度，避免 -π 与 π 的表示差异触发回声下发
                _preTransform = (pos, transform.eulerAngles.z * Mathf.Deg2Rad, _preTransform.scale);
                _transformSynced = true;
            }
        }
    }
}
