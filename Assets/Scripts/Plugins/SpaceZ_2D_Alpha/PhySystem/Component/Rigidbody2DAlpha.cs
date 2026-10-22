using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    [DefaultExecutionOrder(-10)]
    public class Rigidbody2DAlpha : PhySystem.Rigidbody2DAlpha {}
    public static partial class PhySystem
    {
        public class Rigidbody2DAlpha : NativeComponent<Rigidbody2DAlpha>, IOnPhyUpdate
        {
            public Vector2 LinearVelocity
            {
                get => _linearVelocity;
                set
                {
                    if(value == _linearVelocity)
                        return;
                    SetVelocity(value, _angularVelocity);
                    _linearVelocity = value;
                }
            }
            public float AngularVelocity
            {
                get => _angularVelocity;
                set
                {
                    if(value == _angularVelocity)
                        return;
                    SetVelocity(_linearVelocity, value);
                    _angularVelocity = value;
                }
            }
            
            public float Mass
            {
                get => _mass;
                set
                {
                    value = Mathf.Max(0f, value);
                    if(value == _mass)
                        return;
                    SetBody(value, _inertia);
                    _mass = value;
                }
            }
            public float Inertia
            {
                get => _inertia;
                set
                {
                    value = Mathf.Max(0f, value);
                    if(value == _inertia)
                        return;
                    SetBody(_mass, value);
                    _inertia = value;
                }
            }

            public float LinearDrag
            {
                get => _linearDrag;
                set
                {
                    value = Mathf.Clamp01(value);
                    if(value == _linearDrag)
                        return;
                    SetBodyDrag(value, _angularDrag);
                    _linearDrag = value;
                }
            }
            public float AngularDrag
            {
                get => _angularDrag;
                set
                {
                    value = Mathf.Clamp01(value);
                    if(value == _angularDrag)
                        return;
                    SetBodyDrag(_linearDrag, value);
                    _angularDrag = value;
                }
            }

            private float _angularVelocity;
            private Vector2 _linearVelocity;
            [SerializeField]
            private float _mass = 1f;
            [SerializeField]
            private float _inertia = 1f;

            [SerializeField]
            private float _linearDrag;
            [SerializeField]
            private float _angularDrag = 0.05f;

            private (Vector2 pos, float rot, Vector2 scale) _preTransform;

            void Awake()
            {
                CreateBody();
                SetBody(_mass, _inertia);
                SetBodyDrag(_linearDrag, _angularDrag);
            }
            public void OnPhyUpdate()
            {
                Vector2 currPos = new(){ x = transform.position.x, y = transform.position.y};
                float currRot = transform.eulerAngles.z * Mathf.Deg2Rad;
                Vector2 currScale = new(){ x = transform.lossyScale.x, y = transform.lossyScale.y};
                
                if(currPos != _preTransform.pos
                || currRot != _preTransform.rot
                || currScale != _preTransform.scale)
                    SetTransform(currPos, currRot, currScale);

                _preTransform = (currPos, currRot, currScale);
            }
            void OnDestroy()
            => DestroyBody();

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
                Debug.Log($"【刚体】创建刚体{_handle}");
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
                Debug.Log($"【刚体】销毁刚体{_handle}");
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
            private void SetBodyDrag(float lDrag, float aDrag)
            {
                if(!IsEnable(_bodies))
                    return;
                _bodyDragBuffer.Add(new()
                {
                    handle = _handle,
                    linearDrag = lDrag,
                    angularDrag = aDrag,
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

            public void SetState(BodyState state)
            {
                if(!IsEnable(_bodies) || state.handle != _handle)
                    return;
                transform.rotation = Quaternion.AngleAxis(state.rotation * Mathf.Rad2Deg, Vector3.forward);
                transform.position = new() { x = state.position.x, y = state.position.y, z = transform.position.z};
                _angularVelocity = state.angularVelocity;
                _linearVelocity = state.linearVelocity;

                _preTransform = (state.position, state.rotation, _preTransform.scale);
            }
        }
    }
}