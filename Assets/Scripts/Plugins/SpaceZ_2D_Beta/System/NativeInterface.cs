using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 原生接口的函数指针声明（仅编辑器内运行期绑定用到）
    /// <para>与 PhySystemNative.cs 的 DllImport 分支一一对应：同一份签名、同一个 EntryPoint。</para>
    /// <para>缓冲区 API（CreateBuffer / DestroyBuffer）不在这里：它作为 <see cref="NativeArray"/> 的静态映射，
    /// 由 <see cref="NativeArray"/> 自己声明与绑定。</para>
    /// <para>约定：返回 bool 的函数必须标 [return: MarshalAs(UnmanagedType.I1)]，
    /// 原生侧是 1 字节 bool，默认 4 字节 BOOL 封送会读到高位垃圾。</para>
    /// </summary>
    public static partial class PhySystem
    {
#if UNITY_EDITOR
        #region 引擎接口
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void StartEngineFunc(int subStep, int iteration);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void StopEngineFunc();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void StepFunc(float dt);

        /// <summary>取本帧脏刚体（NativeArray&lt;BodyState&gt;，用完须 DestroyBuffer）</summary>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray DirtyBodiesFunc();

        /// <summary>取本帧新进入的碰撞对（NativeArray&lt;CollisionPair&gt;）</summary>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray EnterCollisionPairsFunc();

        /// <summary>取本帧持续中的碰撞对（NativeArray&lt;CollisionPair&gt;）</summary>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray StayCollisionPairsFunc();

        /// <summary>取本帧结束的碰撞对（NativeArray&lt;CollisionObj&gt;）</summary>
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray ExitPairsFunc();
        #endregion

        #region 物理对象
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate Handle CreateBodyFunc();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate Handle CreateColliderFunc(ColliderType type);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private delegate bool DestroyBodyFunc(Handle handle);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private delegate bool DestroyColliderFunc(Handle handle);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void SetColliderBodyFunc(Handle collider, Handle body);
        #endregion

        #region 刚体批量写入
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetBodiesPropertyFunc(BodyProperty* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetBodiesVelocityFunc(BodyVelocity* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void AddForcesFunc(ForceInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void AddTorquesFunc(TorqueInfo* infos, int count);
        #endregion

        #region 碰撞体批量写入
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetCollidersPropertyFunc(ColliderProperty* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetCollidersCollisionFunc(ColliderCollision* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetBodiesTransformFunc(TransformInfo* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetCollidersTransformFunc(TransformInfo* infos, int count);
        #endregion

        #region 碰撞形状批量写入
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetLineCollidersFunc(LineLength* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetBoxCollidersFunc(BoxExtents* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetSphereCollidersFunc(SphereRadius* infos, int count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private unsafe delegate void SetCapsuleCollidersFunc(CapsuleSize* infos, int count);
        #endregion

        #region 空间查询
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray OverlapBoxFunc(Vector2 center, Vector2 size, float rotation, ulong layer);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray OverlapSphereFunc(Vector2 center, float radius, float rotation, ulong layer);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray OverlapCapsuleFunc(Vector2 center, float radius, float height, float rotation,
                                                        ulong layer);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        private unsafe delegate bool RaycastFunc(Vector2 origin, Vector2 direction, RaycastHitInfo* hit, float maxDis,
                                                 ulong layer);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray RaycastAllFunc(Vector2 origin, Vector2 direction, float maxDis, ulong layer);
        #endregion
#endif
    }
}
