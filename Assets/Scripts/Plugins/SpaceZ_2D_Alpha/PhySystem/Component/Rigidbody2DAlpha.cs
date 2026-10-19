using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public class Rigidbody2DAlpha : PhySystem.Rigidbody2DAlpha {}
    public static partial class PhySystem
    {
        public class Rigidbody2DAlpha : NativeComponent<Rigidbody2DAlpha>
        {
            private void CreateBody()
            {
                var handle = PhySystem.CreateBody();
                if(handle == Handle.NULL)
                {
                    Debug.LogError("【刚体】刚体创建失败");
                    return;
                }
                if(_bodies.ContainsKey(handle))
                    throw new Exception($"【刚体】刚体句柄冲突{handle}");
                _bodies.Add(handle, this);
                _handle = handle;

                _dynamicColliderMap.Add(handle, new());
            }
            private void DestroyBody()
            {
                if(!IsEnable(_bodies))
                    return;

                if(_dynamicColliderMap.TryGetValue(_handle, out var set))
                {
                    var arr = set.ToArray();
                    foreach(var cHandle in arr)
                        if(_colliders.TryGetValue(cHandle, out var collider))
                            collider?.DestroyCollider();
                    _dynamicColliderMap.Remove(_handle);
                }
                PhySystem.DestroyBody(_handle);
                _bodies.Remove(_handle);

                _handle = Handle.NULL;
            }
            private void SetBody(float mass, float inertia)
            {
                if(!IsEnable(_bodies))
                    return;
                _bodyMassBuffer.Add(new()
                {
                    handle = _handle,
                    mass = mass,
                    inertia = inertia,
                });
            }
            private void SetVelocity(Vector2 lv, float av)
            {
                if(!IsEnable(_bodies))
                    return;
                _bodyVelBuffer.Add(new()
                {
                    handle = _handle,
                    linearVelocity = lv,
                    angularVelocity = av,
                });
            }
            private void SetTransform(Vector2 pos, float rot, Vector2 scale)
            {
                if(!IsEnable(_bodies))
                    return;
                _bodyTransformBuffer.Add(new()
                {
                    handle = _handle,
                    position = pos,
                    rotation = rot,
                    scale = scale,
                });
            }
            public void AddForce(Vector2 f)
            {
                if(!IsEnable(_bodies))
                    return;
                _bodyForceBuffer.Add(new()
                {
                    handle = _handle,
                    force = f,
                });
            }
            public void AddTorque(float t)
            {
                if(!IsEnable(_bodies))
                    return;
                _bodyTorqueBuffer.Add(new()
                {
                    handle = _handle,
                    torque = t,
                });
            }
        }
    }
}