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
        private static ReadOnlyNativeBuffer<BodyVelocityInfo> _bodyVelBuffer = new();
        private static ReadOnlyNativeBuffer<ColliderMaterialInfo> _colliderMatBuffer = new();
        private static ReadOnlyNativeBuffer<ColliderTransformInfo> _colliderTransformBuffer = new();
        private static ReadOnlyNativeBuffer<BodyTransformInfo> _bodyTransformBuffer = new();
        private static ReadOnlyNativeBuffer<BoxColliderInfo> _boxColliderBuffer = new();
        private static ReadOnlyNativeBuffer<SphereColliderInfo> _sphereColliderBuffer = new();
        private static ReadOnlyNativeBuffer<CapsuleColliderInfo> _capsuleColliderBuffer = new();

        private static ReadOnlyNativeBuffer<BodyForceInfo> _bodyForceBuffer = new();
        private static ReadOnlyNativeBuffer<BodyTorqueInfo> _bodyTorqueBuffer = new();

        [RuntimeStart(-90)]
        private static void InitManaged()
        {
            StartEngine(SUB_STEP_COUNT, ITERATION);

            _bodyMassBuffer.Init();
            _bodyVelBuffer.Init();
            _colliderMatBuffer.Init();
            _colliderTransformBuffer.Init();
            _bodyTransformBuffer.Init();
            _boxColliderBuffer.Init();
            _sphereColliderBuffer.Init();
            _capsuleColliderBuffer.Init();

            _bodyForceBuffer.Init();
            _bodyTorqueBuffer.Init();
        }
        [RuntimeEnd(90)]
        private static void OnApplicationQuitManaged()
        {
            StopEngine();

            _bodyMassBuffer.Dispose();
            _bodyVelBuffer.Dispose();
            _colliderMatBuffer.Dispose();
            _colliderTransformBuffer.Dispose();
            _bodyTransformBuffer.Dispose();
            _boxColliderBuffer.Dispose();
            _sphereColliderBuffer.Dispose();
            _capsuleColliderBuffer.Dispose();

            _bodyForceBuffer.Dispose();
            _bodyTorqueBuffer.Dispose();
        }
        private static void SyncDirtyData()
        {
            unsafe
            {
                _bodyMassBuffer.SyncAndClear(SetBodies);
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
    }
}