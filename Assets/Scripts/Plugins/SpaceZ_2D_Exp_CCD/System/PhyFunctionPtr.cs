using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SpaceZ_2D.Exp.CCD
{
    public static class PhyFunctionPtr
    {
#if UNITY_EDITOR
    #region 引擎接口
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void StartEngine(int subStep, int iteration);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void StopEngine();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void Step(float dt);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int GetDirtyBodyCount();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void GetDirtyBodies(ExportBodyInfo* infos, ref int count);
    #endregion

    #region 物理对象
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate Handle CreateBody();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate Handle CreateCollider(Handle body, ColliderType type);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void DestroyBody(Handle handle);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void DestroyCollider(Handle handle);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void SetColliderRigidbody(Handle collider, Handle body);
    #endregion

    #region 批量数据同步
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBodiesProperty(BodyProperty* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBodiesDrag(BodyDrag* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBodiesVelocity(BodyVelocity* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetCollidersProperty(ColliderProperty* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBodiesTransform(ExportTransform* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetCollidersTransform(ExportTransform* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetBoxColliders(BoxExtents* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetSphereColliders(SphereRadius* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void SetCapsuleColliders(CapsuleSize* infos, int count);
    #endregion

    #region 刚体加力
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void AddForces(BodyForce* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate void AddTorques(BodyTorque* infos, int count);
    #endregion
#endif
    }
}