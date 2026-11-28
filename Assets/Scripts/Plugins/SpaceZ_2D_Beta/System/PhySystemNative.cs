using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using SpaceZ_2D.Framework.Plugin;
using SpaceZ_2D.Framework.RunTime;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 原生接口绑定层：只做「C# 签名 ↔ 导出符号」的映射，不含任何托管侧语义
    /// <para>编辑器内走 AutoNativeLoader 运行期绑定（改 .dll 免重启），其它运行时走 DllImport。</para>
    /// <para>与 PhySystemNative.cs 里两张表必须严格同步：EntryPoint 名字、参数类型、bool 封送约定。</para>
    /// </summary>
    public static partial class PhySystem
    {
        public const string PLUGIN_NAME = "SpaceZ_2D_Beta";

#if UNITY_EDITOR
        private static AutoNativeLoader _loader;

    #region 引擎接口
        [NativeFunc("StartEngine")] private static StartEngineFunc _startEngine;
        [NativeFunc("StopEngine")] private static StopEngineFunc _stopEngine;
        [NativeFunc("Step")] private static StepFunc _engineStep;

        [NativeFunc("DirtyBodies")] private static DirtyBodiesFunc _dirtyBodies;
        [NativeFunc("EnterCollisionPairs")] private static EnterCollisionPairsFunc _enterCollisionPairs;
        [NativeFunc("StayCollisionPairs")] private static StayCollisionPairsFunc _stayCollisionPairs;
        [NativeFunc("ExitPairs")] private static ExitPairsFunc _exitPairs;
    #endregion

    #region 物理对象
        [NativeFunc("CreateBody")] private static CreateBodyFunc _createBody;
        [NativeFunc("CreateCollider")] private static CreateColliderFunc _createCollider;
        [NativeFunc("DestroyBody")] private static DestroyBodyFunc _destroyBody;
        [NativeFunc("DestroyCollider")] private static DestroyColliderFunc _destroyCollider;
        [NativeFunc("SetColliderBody")] private static SetColliderBodyFunc _setColliderBody;
    #endregion

    #region 刚体批量写入
        [NativeFunc("SetBodiesProperty")] private static SetBodiesPropertyFunc _setBodiesProperty;
        [NativeFunc("SetBodiesVelocity")] private static SetBodiesVelocityFunc _setBodiesVelocity;
        [NativeFunc("AddForces")] private static AddForcesFunc _addForces;
        [NativeFunc("AddTorques")] private static AddTorquesFunc _addTorques;
    #endregion

    #region 碰撞体批量写入
        [NativeFunc("SetCollidersProperty")] private static SetCollidersPropertyFunc _setCollidersProperty;
        [NativeFunc("SetCollidersCollision")] private static SetCollidersCollisionFunc _setCollidersCollision;
        [NativeFunc("SetBodiesTransform")] private static SetBodiesTransformFunc _setBodiesTransform;
        [NativeFunc("SetCollidersTransform")] private static SetCollidersTransformFunc _setCollidersTransform;
    #endregion

    #region 碰撞形状批量写入
        [NativeFunc("SetLineColliders")] private static SetLineCollidersFunc _setLineColliders;
        [NativeFunc("SetBoxColliders")] private static SetBoxCollidersFunc _setBoxColliders;
        [NativeFunc("SetSphereColliders")] private static SetSphereCollidersFunc _setSphereColliders;
        [NativeFunc("SetCapsuleColliders")] private static SetCapsuleCollidersFunc _setCapsuleColliders;
    #endregion

    #region 空间查询
        [NativeFunc("OverlapBox")] private static OverlapBoxFunc _overlapBox;
        [NativeFunc("OverlapSphere")] private static OverlapSphereFunc _overlapSphere;
        [NativeFunc("OverlapCapsule")] private static OverlapCapsuleFunc _overlapCapsule;
        [NativeFunc("Raycast")] private static RaycastFunc _raycast;
        [NativeFunc("RaycastAll")] private static RaycastAllFunc _raycastAll;
    #endregion

        [RuntimeStart(-100)]
        private static void InitNative()
        {
            try
            {
                _loader = new AutoNativeLoader(PLUGIN_NAME);
                _loader.Load(typeof(PhySystem));
                // 缓冲区 API 挂在 NativeArray 上，需单独扫一遍它的 NativeFunc 字段
                _loader.Load(typeof(NativeArray));
            }
            catch (Exception ex)
            {
                Debug.LogError($"【PhySystem】Failed to load native plugin: {PLUGIN_NAME}. Exception: {ex.Message}");
                return;
            }
            Debug.Log($"【PhySystem】Native plugin loaded: {PLUGIN_NAME}");
        }
        [RuntimeEnd(100)]
        private static void DestroyNative()
        {
            if (_loader == null)
                return;
            _loader.UnLoad(typeof(PhySystem));
            _loader.UnLoad(typeof(NativeArray));
            _loader.Dispose();
            _loader = null;
        }

    #region 引擎接口
        private static void StartEngine(int subStep, int iteration)
        => _startEngine?.Invoke(subStep, iteration);
        private static void StopEngine() => _stopEngine?.Invoke();
        private static void Step(float dt) => _engineStep?.Invoke(dt);

        private static NativeArray DirtyBodies()
        => _dirtyBodies?.Invoke() ?? NativeArray.NULL;
        private static NativeArray EnterCollisionPairs()
        => _enterCollisionPairs?.Invoke() ?? NativeArray.NULL;
        private static NativeArray StayCollisionPairs()
        => _stayCollisionPairs?.Invoke() ?? NativeArray.NULL;
        private static NativeArray ExitPairs()
        => _exitPairs?.Invoke() ?? NativeArray.NULL;
    #endregion

    #region 物理对象
        private static Handle CreateBody()
        => _createBody?.Invoke() ?? Handle.NULL;
        private static Handle CreateCollider(ColliderType type)
        => _createCollider?.Invoke(type) ?? Handle.NULL;
        private static bool DestroyBody(Handle handle)
        => _destroyBody?.Invoke(handle) ?? false;
        private static bool DestroyCollider(Handle handle)
        => _destroyCollider?.Invoke(handle) ?? false;
        private static void SetColliderBody(Handle collider, Handle body)
        => _setColliderBody?.Invoke(collider, body);
    #endregion

    #region 批量写入
        private static unsafe void SetBodiesProperty(BodyProperty* infos, int count)
        => _setBodiesProperty?.Invoke(infos, count);
        private static unsafe void SetBodiesVelocity(BodyVelocity* infos, int count)
        => _setBodiesVelocity?.Invoke(infos, count);
        private static unsafe void AddForces(ForceInfo* infos, int count)
        => _addForces?.Invoke(infos, count);
        private static unsafe void AddTorques(TorqueInfo* infos, int count)
        => _addTorques?.Invoke(infos, count);

        private static unsafe void SetCollidersProperty(ColliderProperty* infos, int count)
        => _setCollidersProperty?.Invoke(infos, count);
        private static unsafe void SetCollidersCollision(ColliderCollision* infos, int count)
        => _setCollidersCollision?.Invoke(infos, count);
        private static unsafe void SetBodiesTransform(TransformInfo* infos, int count)
        => _setBodiesTransform?.Invoke(infos, count);
        private static unsafe void SetCollidersTransform(TransformInfo* infos, int count)
        => _setCollidersTransform?.Invoke(infos, count);

        private static unsafe void SetLineColliders(LineLength* infos, int count)
        => _setLineColliders?.Invoke(infos, count);
        private static unsafe void SetBoxColliders(BoxExtents* infos, int count)
        => _setBoxColliders?.Invoke(infos, count);
        private static unsafe void SetSphereColliders(SphereRadius* infos, int count)
        => _setSphereColliders?.Invoke(infos, count);
        private static unsafe void SetCapsuleColliders(CapsuleSize* infos, int count)
        => _setCapsuleColliders?.Invoke(infos, count);
    #endregion

    #region 空间查询
        private static NativeArray OverlapBox(Vector2 center, Vector2 size, float rotation, ulong layer)
        => _overlapBox?.Invoke(center, size, rotation, layer) ?? NativeArray.NULL;
        private static NativeArray OverlapSphere(Vector2 center, float radius, float rotation, ulong layer)
        => _overlapSphere?.Invoke(center, radius, rotation, layer) ?? NativeArray.NULL;
        private static NativeArray OverlapCapsule(Vector2 center, float radius, float height, float rotation,
                                                 ulong layer)
        => _overlapCapsule?.Invoke(center, radius, height, rotation, layer) ?? NativeArray.NULL;
        private static unsafe bool Raycast(Vector2 origin, Vector2 direction, RaycastHitInfo* hit, float maxDis,
                                           ulong layer)
        => _raycast?.Invoke(origin, direction, hit, maxDis, layer) ?? false;
        private static NativeArray RaycastAll(Vector2 origin, Vector2 direction, float maxDis, ulong layer)
        => _raycastAll?.Invoke(origin, direction, maxDis, layer) ?? NativeArray.NULL;
    #endregion
#else
    #region 引擎接口
        [DllImport(PLUGIN_NAME, EntryPoint = "StartEngine")]
        private static extern void StartEngine(int subStep, int iteration);
        [DllImport(PLUGIN_NAME, EntryPoint = "StopEngine")]
        private static extern void StopEngine();
        [DllImport(PLUGIN_NAME, EntryPoint = "Step")]
        private static extern void Step(float dt);

        [DllImport(PLUGIN_NAME, EntryPoint = "DirtyBodies")]
        private static extern NativeArray DirtyBodies();
        [DllImport(PLUGIN_NAME, EntryPoint = "EnterCollisionPairs")]
        private static extern NativeArray EnterCollisionPairs();
        [DllImport(PLUGIN_NAME, EntryPoint = "StayCollisionPairs")]
        private static extern NativeArray StayCollisionPairs();
        [DllImport(PLUGIN_NAME, EntryPoint = "ExitPairs")]
        private static extern NativeArray ExitPairs();
    #endregion

    #region 物理对象
        [DllImport(PLUGIN_NAME, EntryPoint = "CreateBody")]
        private static extern Handle CreateBody();
        [DllImport(PLUGIN_NAME, EntryPoint = "CreateCollider")]
        private static extern Handle CreateCollider(ColliderType type);
        [DllImport(PLUGIN_NAME, EntryPoint = "DestroyBody")]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool DestroyBody(Handle handle);
        [DllImport(PLUGIN_NAME, EntryPoint = "DestroyCollider")]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool DestroyCollider(Handle handle);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetColliderBody")]
        private static extern void SetColliderBody(Handle collider, Handle body);
    #endregion

    #region 批量写入
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesProperty")]
        private static extern unsafe void SetBodiesProperty(BodyProperty* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesVelocity")]
        private static extern unsafe void SetBodiesVelocity(BodyVelocity* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "AddForces")]
        private static extern unsafe void AddForces(ForceInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "AddTorques")]
        private static extern unsafe void AddTorques(TorqueInfo* infos, int count);

        [DllImport(PLUGIN_NAME, EntryPoint = "SetCollidersProperty")]
        private static extern unsafe void SetCollidersProperty(ColliderProperty* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetCollidersCollision")]
        private static extern unsafe void SetCollidersCollision(ColliderCollision* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesTransform")]
        private static extern unsafe void SetBodiesTransform(TransformInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetCollidersTransform")]
        private static extern unsafe void SetCollidersTransform(TransformInfo* infos, int count);

        [DllImport(PLUGIN_NAME, EntryPoint = "SetLineColliders")]
        private static extern unsafe void SetLineColliders(LineLength* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBoxColliders")]
        private static extern unsafe void SetBoxColliders(BoxExtents* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetSphereColliders")]
        private static extern unsafe void SetSphereColliders(SphereRadius* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetCapsuleColliders")]
        private static extern unsafe void SetCapsuleColliders(CapsuleSize* infos, int count);
    #endregion

    #region 空间查询
        [DllImport(PLUGIN_NAME, EntryPoint = "OverlapBox")]
        private static extern NativeArray OverlapBox(Vector2 center, Vector2 size, float rotation, ulong layer);
        [DllImport(PLUGIN_NAME, EntryPoint = "OverlapSphere")]
        private static extern NativeArray OverlapSphere(Vector2 center, float radius, float rotation, ulong layer);
        [DllImport(PLUGIN_NAME, EntryPoint = "OverlapCapsule")]
        private static extern NativeArray OverlapCapsule(Vector2 center, float radius, float height, float rotation,
                                                         ulong layer);
        [DllImport(PLUGIN_NAME, EntryPoint = "Raycast")]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern unsafe bool Raycast(Vector2 origin, Vector2 direction, RaycastHitInfo* hit, float maxDis,
                                                  ulong layer);
        [DllImport(PLUGIN_NAME, EntryPoint = "RaycastAll")]
        private static extern NativeArray RaycastAll(Vector2 origin, Vector2 direction, float maxDis, ulong layer);
    #endregion
#endif
    }
}
