using System.Collections;
using System.Collections.Generic;
using SpaceZ_2D.Framework.RunTime;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public static partial class PhySystem
    {
        public const int SUB_STEP_COUNT = 4;
        public const int ITERATION = 8;
        private static Dictionary<Handle, Collider2DAlpha> _colliders = new();
        private static Dictionary<Handle, Rigidbody2DAlpha> _bodies = new();
        private static Dictionary<Handle, HashSet<Handle>> _dynamicColliderMap = new();

        private static ReadOnlyNativeBuffer<BodyMassInfo> _bodyMassBuffer = new();
        private static ReadOnlyNativeBuffer<BodyDragInfo> _bodyDragBuffer = new();
        private static ReadOnlyNativeBuffer<BodyVelocityInfo> _bodyVelBuffer = new();
        private static ReadOnlyNativeBuffer<ColliderMaterialInfo> _colliderMatBuffer = new();
        private static ReadOnlyNativeBuffer<ColliderTransformInfo> _colliderTransformBuffer = new();
        private static ReadOnlyNativeBuffer<BodyTransformInfo> _bodyTransformBuffer = new();
        private static ReadOnlyNativeBuffer<BoxColliderInfo> _boxColliderBuffer = new();
        private static ReadOnlyNativeBuffer<SphereColliderInfo> _sphereColliderBuffer = new();
        private static ReadOnlyNativeBuffer<CapsuleColliderInfo> _capsuleColliderBuffer = new();

        private static ReadOnlyNativeBuffer<BodyForceInfo> _bodyForceBuffer = new();
        private static ReadOnlyNativeBuffer<BodyTorqueInfo> _bodyTorqueBuffer = new();

        private static NativeDataBuffer<BodyState> _bodyStateBuffer = new();

        private static List<IOnPhyUpdate> _temp = new();

        [RuntimeStart(-90)]
        private static void InitManaged()
        {
            StartEngine(SUB_STEP_COUNT, ITERATION);

            _bodyMassBuffer.Init();
            _bodyDragBuffer.Init();
            _bodyVelBuffer.Init();
            _colliderMatBuffer.Init();
            _colliderTransformBuffer.Init();
            _bodyTransformBuffer.Init();
            _boxColliderBuffer.Init();
            _sphereColliderBuffer.Init();
            _capsuleColliderBuffer.Init();

            _bodyForceBuffer.Init();
            _bodyTorqueBuffer.Init();

            _bodyStateBuffer.Init();

            PhyLateUpdate.Add(Step);
        }
        [RuntimeEnd(90)]
        private static void OnApplicationQuitManaged()
        {
            PhyLateUpdate.Remove(Step);
            
            StopEngine();

            _bodyMassBuffer.Dispose();
            _bodyDragBuffer.Dispose();
            _bodyVelBuffer.Dispose();
            _colliderMatBuffer.Dispose();
            _colliderTransformBuffer.Dispose();
            _bodyTransformBuffer.Dispose();
            _boxColliderBuffer.Dispose();
            _sphereColliderBuffer.Dispose();
            _capsuleColliderBuffer.Dispose();

            _bodyForceBuffer.Dispose();
            _bodyTorqueBuffer.Dispose();

            _bodyStateBuffer.Dispose();
        }
        private static void Step()
        {
            ComponentUpdate();
            SyncDirtyData();
            EngineStep(PhyTime.deltaTime);
            GetDirtyBodies();
        }
        private static void ComponentUpdate()
        {
            _temp.Clear();
            foreach(var body in _bodies.Values)
                if(body is IOnPhyUpdate bUpdate)
                    _temp.Add(bUpdate);
            foreach(var collider in _colliders.Values)
                if(collider is IOnPhyUpdate cUpdate)
                    _temp.Add(cUpdate);
            for(int i = 0; i < _temp.Count; i++)
                _temp[i].OnPhyUpdate();
        }
        private static void SyncDirtyData()
        {
            unsafe
            {
                _bodyMassBuffer.SyncAndClear(SetBodies);
                _bodyDragBuffer.SyncAndClear(SetBodiesDrag);
                _bodyVelBuffer.SyncAndClear(SetBodiesVelocity);
                _colliderMatBuffer.SyncAndClear(SetColliders);
                _colliderTransformBuffer.SyncAndClear(SetCollidersTransform);
                _bodyTransformBuffer.SyncAndClear(SetBodiesTransform);
                _boxColliderBuffer.SyncAndClear(SetBoxColliders);
                _sphereColliderBuffer.SyncAndClear(SetSphereColliders);
                _capsuleColliderBuffer.SyncAndClear(SetCapsuleColliders);

                _bodyForceBuffer.SyncAndClear(AddForce);
                _bodyTorqueBuffer.SyncAndClear(AddTorque);
            }
        }
        private static void GetDirtyBodies()
        {
            int count = GetDirtyBodyCount();
            if(count < 0)
                Debug.LogError("【托管物理系统】更改碰撞体数量获取异常");
            unsafe
            {
                _bodyStateBuffer.GetNativeData(ref count, GetDirtyBody);
            }
            var list = _bodyStateBuffer.NativeDataList;
            foreach(var state in list)
                if(state.handle != Handle.NULL && _bodies.TryGetValue(state.handle, out var body))
                    body?.SetState(state);
        }
    }
}