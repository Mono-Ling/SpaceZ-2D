using System.Collections;
using System.Collections.Generic;
using SpaceZ_2D.Framework.RunTime;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    public static partial class PhySystem
    {
        public const int SUB_STEP_COUNT = 6;
        public const int SOLVER_ITERATION = 8;
        private static Dictionary<Handle, Rigidbody2D_Exp_CCD> _bodies = new();
        private static Dictionary<Handle, Collider2D_Exp_CCD> _colliders = new();
        private static Dictionary<Handle, HashSet<Handle>> _dynamicColliders = new();

        private static NativeWriteBuffer<ExportBodyInfo> _dirtyBodies = new();

        private static NativeReadBuffer<BodyProperty> _dirtyBodiesProperty = new();
        private static NativeReadBuffer<BodyDrag> _dirtyBodiesDrag = new();
        private static NativeReadBuffer<BodyVelocity> _dirtyBodiesVelocity = new();
        private static NativeReadBuffer<ColliderProperty> _dirtyCollidersProperty = new();
        private static NativeReadBuffer<ExportTransform> _dirtyBodiesTransform = new();
        private static NativeReadBuffer<ExportTransform> _dirtyCollidersTransform = new();
        private static NativeReadBuffer<BoxExtents> _dirtyBoxColliders = new();
        private static NativeReadBuffer<SphereRadius> _dirtySphereColliders = new();
        private static NativeReadBuffer<CapsuleSize> _dirtyCapsuleColliders = new();

        private static NativeReadBuffer<BodyForce> _bodyForces = new();
        private static NativeReadBuffer<BodyTorque> _bodyTorques = new();

        private static List<INativeComponent> _nativeComponents = new();

        [RuntimeStart(-90)]
        private static void InitManaged()
        {
            _dirtyBodies.Init();
            _dirtyBodiesProperty.Init();
            _dirtyBodiesDrag.Init();
            _dirtyBodiesVelocity.Init();
            _dirtyCollidersProperty.Init();
            _dirtyBodiesTransform.Init();
            _dirtyCollidersTransform.Init();
            _dirtyBoxColliders.Init();
            _dirtySphereColliders.Init();
            _dirtyCapsuleColliders.Init();
            _bodyForces.Init();
            _bodyTorques.Init();

            StartEngine(SUB_STEP_COUNT, SOLVER_ITERATION);

            PhyLateUpdate.Add(Step);
        }
        [RuntimeEnd(90)]
        private static void DestroyManaged()
        {
            PhyLateUpdate.Remove(Step);

            StopEngine();

            _dirtyBodies.Dispose();
            _dirtyBodiesProperty.Dispose();
            _dirtyBodiesDrag.Dispose();
            _dirtyBodiesVelocity.Dispose();
            _dirtyCollidersProperty.Dispose();
            _dirtyBodiesTransform.Dispose();
            _dirtyCollidersTransform.Dispose();
            _dirtyBoxColliders.Dispose();
            _dirtySphereColliders.Dispose();
            _dirtyCapsuleColliders.Dispose();
            _bodyForces.Dispose();
            _bodyTorques.Dispose();
        }
        private static void Step()
        {
            ComponentUpdate();
            SynDirtyData();
            Step(PhyTime.deltaTime);
            GetDirtyNativeBodies();
        }
        private static void ComponentUpdate()
        {
            _nativeComponents.Clear();
            foreach(var body in _bodies.Values)
                if(body is INativeComponent bodyCom)
                    _nativeComponents.Add(bodyCom);
            foreach(var collider in _colliders.Values)
                if(collider is INativeComponent colliderCom)
                    _nativeComponents.Add(colliderCom);

            foreach(var component in _nativeComponents)
                component?.ComponentUpdate();
        }
        private static void SynDirtyData()
        {
            unsafe
            {
                _dirtyBodiesProperty.SynAndClear(SetBodiesProperty);
                _dirtyBodiesDrag.SynAndClear(SetBodiesDrag);
                _dirtyBodiesVelocity.SynAndClear(SetBodiesVelocity);
                _dirtyCollidersProperty.SynAndClear(SetCollidersProperty);
                _dirtyBodiesTransform.SynAndClear(SetBodiesTransform);
                _dirtyCollidersTransform.SynAndClear(SetCollidersTransform);
                _dirtyBoxColliders.SynAndClear(SetBoxColliders);
                _dirtySphereColliders.SynAndClear(SetSphereColliders);
                _dirtyCapsuleColliders.SynAndClear(SetCapsuleColliders);
                _bodyForces.SynAndClear(AddForces);
                _bodyTorques.SynAndClear(AddTorques);
            }
        }
        private static void GetDirtyNativeBodies()
        {
            int count = GetDirtyBodyCount();
            if(count < 0)
                Debug.LogError($"【Exp CCD 托管物理引擎】脏刚体数量获取异常{count}");
            unsafe
            {
                _dirtyBodies.GetNativeData(ref count, GetDirtyBodies);
            }
        }

        private static Rigidbody2D_Exp_CCD TryGetBody(Handle handle, bool debug = true)
        {
            if(!handle || !_bodies.TryGetValue(handle, out var body))
            {
                if(debug)
                    Debug.LogWarning($"【Exp CCD 托管物理引擎】无效刚体句柄{handle}");
                return null;
            }
            return body;
        }
        private static Collider2D_Exp_CCD TryGetCollider(Handle handle, bool debug = true)
        {
            if(!handle || !_colliders.TryGetValue(handle, out var collider))
            {
                if(debug)
                    Debug.LogWarning($"【Exp CCD 托管物理引擎】无效碰撞体句柄{handle}");
                return null;
            }
            return collider;
        }
    }
}