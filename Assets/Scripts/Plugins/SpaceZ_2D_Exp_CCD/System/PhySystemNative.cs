using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using SpaceZ_2D.Framework.Plugin;
using SpaceZ_2D.Framework.RunTime;
using UnityEngine;
using static SpaceZ_2D.Exp.CCD.PhyFunctionPtr;

namespace SpaceZ_2D.Exp.CCD
{
    public static partial class PhySystem
    {
        public const string PLUGIN_NAME = "SpaceZ_2D_Exp_CCD";

#if UNITY_EDITOR
    #region 引擎接口
        private static AutoNativeLoader _loader;

        [NativeFunc("StartEngine")]
        private static StartEngine _startEngine;

        [NativeFunc("StopEngine")]
        private static StopEngine _stopEngine;

        [NativeFunc("Step")]
        private static Step _engineStep;

        [NativeFunc("GetDirtyBodyCount")]
        private static GetDirtyBodyCount _getDirtyBodyCount;

        [NativeFunc("GetDirtyBodies")]
        private static GetDirtyBodies _getDirtyBodies;
    #endregion

    #region 物理对象
        [NativeFunc("CreateBody")]
        private static CreateBody _createBody;

        [NativeFunc("CreateCollider")]
        private static CreateCollider _createCollider;

        [NativeFunc("DestroyBody")]
        private static DestroyBody _destroyBody;

        [NativeFunc("DestroyCollider")]
        private static DestroyCollider _destroyCollider;

        [NativeFunc("SetColliderRigidbody")]
        private static SetColliderRigidbody _setColliderRigidbody;
    #endregion

    #region 批量数据同步
        [NativeFunc("SetBodiesProperty")]
        private static SetBodiesProperty _setBodiesProperty;

        [NativeFunc("SetBodiesDrag")]
        private static SetBodiesDrag _setBodiesDrag;

        [NativeFunc("SetBodiesVelocity")]
        private static SetBodiesVelocity _setBodiesVelocity;

        [NativeFunc("SetCollidersProperty")]
        private static SetCollidersProperty _setCollidersProperty;

        [NativeFunc("SetBodiesTransform")]
        private static SetBodiesTransform _setBodiesTransform;

        [NativeFunc("SetCollidersTransform")]
        private static SetCollidersTransform _setCollidersTransform;

        [NativeFunc("SetBoxColliders")]
        private static SetBoxColliders _setBoxColliders;

        [NativeFunc("SetSphereColliders")]
        private static SetSphereColliders _setSphereColliders;

        [NativeFunc("SetCapsuleColliders")]
        private static SetCapsuleColliders _setCapsuleColliders;
    #endregion

    #region 刚体加力
        [NativeFunc("AddForces")]
        private static AddForces _addForces;

        [NativeFunc("AddTorques")]
        private static AddTorques _addTorques;
    #endregion

        [RuntimeStart(-100)]
        private static void InitNative()
        {
            try
            {
                _loader = new AutoNativeLoader(PLUGIN_NAME);
                _loader.Load(typeof(PhySystem));
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
            _loader.Dispose();
            _loader = null;
        }

    #region 引擎接口
        private static void StartEngine(int subStep, int iteration)
        => _startEngine?.Invoke(subStep, iteration);
        private static void StopEngine()
        => _stopEngine?.Invoke();
        private static void Step(float dt)
        => _engineStep?.Invoke(dt);
        private static int GetDirtyBodyCount()
        => _getDirtyBodyCount?.Invoke() ?? -1;
        private static unsafe void GetDirtyBodies(ExportBodyInfo* infos, ref int count)
        => _getDirtyBodies?.Invoke(infos, ref count);
    #endregion

    #region 物理对象
        private static Handle CreateBody()
        => _createBody?.Invoke() ?? Handle.NULL;
        private static Handle CreateCollider(Handle body, ColliderType type)
        => _createCollider?.Invoke(body, type) ?? Handle.NULL;
        private static void DestroyBody(Handle handle)
        => _destroyBody?.Invoke(handle);
        private static void DestroyCollider(Handle handle)
        => _destroyCollider?.Invoke(handle);
        private static void SetColliderRigidbody(Handle collider, Handle body)
        => _setColliderRigidbody?.Invoke(collider, body);
    #endregion

    #region 批量数据同步
        private static unsafe void SetBodiesProperty(BodyProperty* infos, int count)
        => _setBodiesProperty?.Invoke(infos, count);
        private static unsafe void SetBodiesDrag(BodyDrag* infos, int count)
        => _setBodiesDrag?.Invoke(infos, count);
        private static unsafe void SetBodiesVelocity(BodyVelocity* infos, int count)
        => _setBodiesVelocity?.Invoke(infos, count);
        private static unsafe void SetCollidersProperty(ColliderProperty* infos, int count)
        => _setCollidersProperty?.Invoke(infos, count);
        private static unsafe void SetBodiesTransform(ExportTransform* infos, int count)
        => _setBodiesTransform?.Invoke(infos, count);
        private static unsafe void SetCollidersTransform(ExportTransform* infos, int count)
        => _setCollidersTransform?.Invoke(infos, count);
        private static unsafe void SetBoxColliders(BoxExtents* infos, int count)
        => _setBoxColliders?.Invoke(infos, count);
        private static unsafe void SetSphereColliders(SphereRadius* infos, int count)
        => _setSphereColliders?.Invoke(infos, count);
        private static unsafe void SetCapsuleColliders(CapsuleSize* infos, int count)
        => _setCapsuleColliders?.Invoke(infos, count);
    #endregion

    #region 刚体加力
        private static unsafe void AddForces(BodyForce* infos, int count)
        => _addForces?.Invoke(infos, count);
        private static unsafe void AddTorques(BodyTorque* infos, int count)
        => _addTorques?.Invoke(infos, count);
    #endregion
#else
        [DllImport(PLUGIN_NAME, EntryPoint = "StartEngine")]
        private static extern void StartEngine(int subStep, int iteration);
        [DllImport(PLUGIN_NAME, EntryPoint = "StopEngine")]
        private static extern void StopEngine();
        [DllImport(PLUGIN_NAME, EntryPoint = "Step")]
        private static extern void Step(float dt);
        [DllImport(PLUGIN_NAME, EntryPoint = "GetDirtyBodyCount")]
        private static extern int GetDirtyBodyCount();
        [DllImport(PLUGIN_NAME, EntryPoint = "GetDirtyBodies")]
        private static extern unsafe void GetDirtyBodies(ExportBodyInfo* infos, ref int count);

        [DllImport(PLUGIN_NAME, EntryPoint = "CreateBody")]
        private static extern Handle CreateBody();
        [DllImport(PLUGIN_NAME, EntryPoint = "CreateCollider")]
        private static extern Handle CreateCollider(Handle body, ColliderType type);
        [DllImport(PLUGIN_NAME, EntryPoint = "DestroyBody")]
        private static extern void DestroyBody(Handle handle);
        [DllImport(PLUGIN_NAME, EntryPoint = "DestroyCollider")]
        private static extern void DestroyCollider(Handle handle);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetColliderRigidbody")]
        private static extern void SetColliderRigidbody(Handle collider, Handle body);

        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesProperty")]
        private static extern unsafe void SetBodiesProperty(BodyProperty* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesDrag")]
        private static extern unsafe void SetBodiesDrag(BodyDrag* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesVelocity")]
        private static extern unsafe void SetBodiesVelocity(BodyVelocity* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetCollidersProperty")]
        private static extern unsafe void SetCollidersProperty(ColliderProperty* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesTransform")]
        private static extern unsafe void SetBodiesTransform(ExportTransform* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetCollidersTransform")]
        private static extern unsafe void SetCollidersTransform(ExportTransform* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBoxColliders")]
        private static extern unsafe void SetBoxColliders(BoxExtents* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetSphereColliders")]
        private static extern unsafe void SetSphereColliders(SphereRadius* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetCapsuleColliders")]
        private static extern unsafe void SetCapsuleColliders(CapsuleSize* infos, int count);

        [DllImport(PLUGIN_NAME, EntryPoint = "AddForces")]
        private static extern unsafe void AddForces(BodyForce* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "AddTorques")]
        private static extern unsafe void AddTorques(BodyTorque* infos, int count);
#endif
    }
}