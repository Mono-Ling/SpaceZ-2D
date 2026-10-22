using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public static class PhySystemFunction
    {
#if UNITY_EDITOR
        #region 引擎生命周期
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void StartEngine(int subStep, int iteration);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void StopEngine();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void EngineStep(float dt);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int GetCollisionCount();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void GetCollisionPairs(CollisionPair* pairs, ref int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int GetDirtyBodyCount();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void GetDirtyBody(BodyState* infos, ref int count);
        #endregion

        #region 物理对象
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate Handle CreateBody();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate Handle CreateCollider(ColliderType type, Handle body);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void DestroyBody(Handle handle);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void DestroyCollider(Handle handle);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void SetColliderBody(Handle handle, Handle body);
        #endregion

        #region 数据批量同步
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBodies(BodyMassInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBodiesDrag(BodyDragInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBodiesVelocity(BodyVelocityInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetColliders(ColliderMaterialInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBodiesTransform(BodyTransformInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetCollidersTransform(ColliderTransformInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBoxColliders(BoxColliderInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetSphereColliders(SphereColliderInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetCapsuleColliders(CapsuleColliderInfo* infos, int count);
        #endregion

        #region 刚体加力
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void AddForce(BodyForceInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void AddTorque(BodyTorqueInfo* infos, int count);
        #endregion
#endif
    }
}
