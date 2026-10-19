using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public static partial class PhySystem
    {
        public abstract class Collider2DAlpha : NativeComponent<Collider2DAlpha>
        {
            private Handle _body = Handle.NULL;
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