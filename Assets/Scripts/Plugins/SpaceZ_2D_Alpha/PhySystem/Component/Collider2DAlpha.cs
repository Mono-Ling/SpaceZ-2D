using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public static partial class PhySystem
    {
        public abstract class Collider2DAlpha : NativeComponent<Collider2DAlpha>, IOnPhyUpdate
        {
            public float StaticFriction
            {
                get => _staticFriction;
                set
                {
                    value = Mathf.Max(0f, value);
                    if(value == _staticFriction)
                        return;
                    SetCollider(value, _dynamicFriction, _elasticity);
                    _staticFriction = value;
                }
            }
            public float DynamicFriction
            {
                get => _dynamicFriction;
                set
                {
                    value = Mathf.Max(0f, value);
                    if(value == _dynamicFriction)
                        return;
                    SetCollider(_staticFriction, value, _elasticity);
                    _dynamicFriction = value;
                }
            }
            public float Elasticity
            {
                get => _elasticity;
                set
                {
                    value = Mathf.Clamp01(value);
                    if(value == _elasticity)
                        return;
                    SetCollider(_staticFriction, _dynamicFriction, value);
                    _elasticity = value;
                }
            }
            private float _staticFriction;
            private float _dynamicFriction;
            private float _elasticity = 0.5f;

            private Transform _bodyObj;
            private Handle _body = Handle.NULL;
            
            private (Vector2 pos, float rot, Vector2 scale) _preTransform;
            void OnTransformParentChanged()
            {
                var obj = GetComponentInParent<Rigidbody2DAlpha>();
                var curr = obj?.transform ?? null;
                if(curr = _bodyObj)
                    return;
                UpdateTransform();
            }
            public void OnPhyUpdate() => UpdateTransform();
            void OnDisable() => DestroyCollider();
            void OnDestroy() => DestroyCollider();
            private void UpdateTransform()
            {
                if(_handle == Handle.NULL)
                    return;
                Matrix4x4 m = (_bodyObj?.worldToLocalMatrix ?? Matrix4x4.identity) * transform.localToWorldMatrix;

                var pos = m.GetPosition();
                var scale = m.lossyScale;
                Vector2 currPos = new(){ x = pos.x, y = pos.y };
                float currRot = m.rotation.eulerAngles.z * Mathf.Deg2Rad;
                Vector2 currScale = new(){ x = scale.x, y = scale.y };

                if(currPos != _preTransform.pos
                || currRot != _preTransform.rot
                || currScale != _preTransform.scale)
                    SetTransform(currPos, currRot, currScale);

                _preTransform = (currPos, currRot, currScale);
            }

            protected void CreateCollider(ColliderType type)
            {
                Handle handle = PhySystem.CreateCollider(type, _body);
                if(handle == Handle.NULL)
                {
                    Debug.LogError("【碰撞体】碰撞体创建失败");
                    return;
                }
                if(_colliders.ContainsKey(handle))
                    throw new Exception($"【碰撞体】碰撞体句柄冲突{handle}");
                _colliders.Add(handle, this);
                _handle = handle;

                if(_dynamicColliderMap.TryGetValue(_body, out var set))
                    set.Add(_handle);
            }
            public void DestroyCollider()
            {
                if(!IsEnable(_colliders))
                    return;
                PhySystem.DestroyCollider(_handle);
                _colliders.Remove(_handle);

                if(_dynamicColliderMap.TryGetValue(_body, out var set))
                    set.Remove(_handle);

                _handle = Handle.NULL;
                _body = Handle.NULL;
            }
            protected void SetBody(Handle body)
            {
                if(!IsEnable(_colliders) || body == _body
                || body != Handle.NULL && !_bodies.ContainsKey(body))
                    return;
                PhySystem.SetColliderBody(_handle, body);
                
                if(_dynamicColliderMap.TryGetValue(_body, out var oldSet))
                    oldSet.Remove(_handle);
                if(_dynamicColliderMap.TryGetValue(body, out var newSet))
                    newSet.Add(_handle);

                _body = body;
            }
            protected void SetCollider(float sf, float dy, float e)
            {
                if(!IsEnable(_colliders))
                    return;
                _colliderMatBuffer.Add(new()
                {
                    handle = _handle,
                    staticFriction = sf,
                    dynamicFriction = dy,
                    elasticity = e,
                });
            }
            protected void SetTransform(Vector2 pos, float rot, Vector2 scale)
            {
                if(!IsEnable(_colliders))
                    return;
                _colliderTransformBuffer.Add(new()
                {
                    handle = _handle,
                    position = pos,
                    rotation = rot,
                    scale = scale,
                });
            }
            protected void SetBox(Vector2 extents)
            {
                if(!IsEnable(_colliders))
                    return;
                _boxColliderBuffer.Add(new()
                {
                    handle = _handle,
                    extents = extents,
                });
            }
            protected void SetSphere(float r)
            {
                if(!IsEnable(_colliders))
                    return;
                _sphereColliderBuffer.Add(new()
                {
                    handle = _handle,
                    radius = r,
                });
            }
            protected void SetCapsule(float h, float r)
            {
                if(!IsEnable(_colliders))
                    return;
                _capsuleColliderBuffer.Add(new()
                {
                    handle = _handle,
                    height = h,
                    radius = r,
                });
            }
        }
    }
}