using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using SpaceZ_2D.Framework.Plugin;
using SpaceZ_2D.Framework.RunTime;
using UnityEngine;

using static SpaceZ_2D.Alpha.PhySystemFunction;

namespace SpaceZ_2D.Alpha
{
    public static partial class PhySystem
    {
        private const string PLUGIN_NAME = "SpaceZ_2D_Alpha";
#if UNITY_EDITOR
        private static NativeLoader _nativeLoader;

        #region 引擎生命周期
        private static StartEngine _startEngine;
        private static StopEngine _stopEngine;
        private static EngineStep _engineStep;
        private static GetCollisionCount _getCollisionCount;
        private static GetCollisionPairs _getCollisionPairs;
        private static GetDirtyBodyCount _getDirtyBodyCount;
        private static GetDirtyBody _getBirtyBody;
        #endregion

        #region 物理对象
        private static CreateBody _createBody;
        private static CreateCollider _createCollider;
        private static DestroyBody _destroyBody;
        private static DestroyCollider _destroyCollider;
        private static SetColliderBody _setColliderBody;
        #endregion

        #region 数据批量同步
        private static SetBodies _setBodies;
        private static SetBodiesDrag _setBodiesDrag;
        private static SetBodiesVelocity _setBodiesVelocity;
        private static SetColliders _setColliders;
        private static SetBodiesTransform _setBodiesTransform;
        private static SetCollidersTransform _setCollidersTransform;
        private static SetBoxColliders _setBoxColliders;
        private static SetSphereColliders _setSphereColliders;
        private static SetCapsuleColliders _setCapsuleColliders;
        #endregion

        #region 刚体加力
        private static AddForce _addForce;
        private static AddTorque _addTorque;
        #endregion

        [RuntimeStart(-100)]
        private static void Init()
        {
            try
            {
                _nativeLoader = new(PLUGIN_NAME);
                
                _nativeLoader.GetFunction(typeof(StartEngine).Name, out _startEngine);
                _nativeLoader.GetFunction(typeof(StopEngine).Name, out _stopEngine);
                _nativeLoader.GetFunction(typeof(EngineStep).Name, out _engineStep);
                _nativeLoader.GetFunction(typeof(GetCollisionCount).Name, out _getCollisionCount);
                _nativeLoader.GetFunction(typeof(GetCollisionPairs).Name, out _getCollisionPairs);
                _nativeLoader.GetFunction(typeof(GetDirtyBodyCount).Name, out _getDirtyBodyCount);
                _nativeLoader.GetFunction(typeof(GetDirtyBody).Name, out _getBirtyBody);

                _nativeLoader.GetFunction(typeof(CreateBody).Name, out _createBody);
                _nativeLoader.GetFunction(typeof(CreateCollider).Name, out _createCollider);
                _nativeLoader.GetFunction(typeof(DestroyBody).Name, out _destroyBody);
                _nativeLoader.GetFunction(typeof(DestroyCollider).Name, out _destroyCollider);
                _nativeLoader.GetFunction(typeof(SetColliderBody).Name, out _setColliderBody);

                _nativeLoader.GetFunction(typeof(SetBodies).Name, out _setBodies);
                _nativeLoader.GetFunction(typeof(SetBodiesDrag).Name, out _setBodiesDrag);
                _nativeLoader.GetFunction(typeof(SetBodiesVelocity).Name, out _setBodiesVelocity);
                _nativeLoader.GetFunction(typeof(SetColliders).Name, out _setColliders);
                _nativeLoader.GetFunction(typeof(SetBodiesTransform).Name, out _setBodiesTransform);
                _nativeLoader.GetFunction(typeof(SetCollidersTransform).Name, out _setCollidersTransform);
                _nativeLoader.GetFunction(typeof(SetBoxColliders).Name, out _setBoxColliders);
                _nativeLoader.GetFunction(typeof(SetSphereColliders).Name, out _setSphereColliders);
                _nativeLoader.GetFunction(typeof(SetCapsuleColliders).Name, out _setCapsuleColliders);

                _nativeLoader.GetFunction(typeof(AddForce).Name, out _addForce);
                _nativeLoader.GetFunction(typeof(AddTorque).Name, out _addTorque);
            }
            catch (System.Exception ex)
            {
                Debug.LogError($"【PhySystemNative】获取函数失败：{ex}");
            }
        }
        [RuntimeEnd(100)]
        private static void OnApplicationQuit()
        {
            _nativeLoader?.Dispose();
            _nativeLoader = null;

            _startEngine = null;
            _stopEngine = null;
            _engineStep = null;
            _getCollisionCount = null;
            _getCollisionPairs = null;
            _getDirtyBodyCount = null;
            _getBirtyBody = null;

            _createBody = null;
            _createCollider = null;
            _destroyBody = null;
            _destroyCollider = null;
            _setColliderBody = null;

            _setBodies = null;
            _setBodiesDrag = null;
            _setBodiesVelocity = null;
            _setColliders = null;
            _setBodiesTransform = null;
            _setCollidersTransform = null;
            _setBoxColliders = null;
            _setSphereColliders = null;
            _setCapsuleColliders = null;

            _addForce = null;
            _addTorque = null;
        }

        #region 引擎生命周期
        private static void StartEngine(int subStep, int iteration)
        => _startEngine?.Invoke(subStep, iteration);
        private static void StopEngine() => _stopEngine?.Invoke();
        private static void EngineStep(float dt) => _engineStep?.Invoke(dt);
        private static int GetCollisionCount() => _getCollisionCount?.Invoke() ?? -1;
        private static unsafe void GetCollisionPairs(CollisionPair* pairs, ref int count)
        => _getCollisionPairs?.Invoke(pairs, ref count);
        private static int GetDirtyBodyCount() => _getDirtyBodyCount?.Invoke() ?? -1;
        private static unsafe void GetDirtyBody(BodyState* infos, ref int count)
        => _getBirtyBody?.Invoke(infos, ref count);
        #endregion

        #region 物理对象
        private static Handle CreateBody() => _createBody?.Invoke() ?? Handle.NULL;
        private static Handle CreateCollider(ColliderType type, Handle body)
        => _createCollider?.Invoke(type, body) ?? Handle.NULL;
        private static void DestroyBody(Handle handle) => _destroyBody?.Invoke(handle);
        private static void DestroyCollider(Handle handle) => _destroyCollider?.Invoke(handle);
        private static void SetColliderBody(Handle handle, Handle body) => _setColliderBody?.Invoke(handle, body);
        #endregion

        #region 数据批量同步
        private static unsafe void SetBodies(BodyMassInfo* infos, int count)
        => _setBodies?.Invoke(infos, count);
        private static unsafe void SetBodiesDrag(BodyDragInfo* infos, int count)
        => _setBodiesDrag?.Invoke(infos, count);
        private static unsafe void SetBodiesVelocity(BodyVelocityInfo* infos, int count)
        => _setBodiesVelocity?.Invoke(infos, count);
        private static unsafe void SetColliders(ColliderMaterialInfo* infos, int count)
        => _setColliders?.Invoke(infos, count);
        private static unsafe void SetBodiesTransform(BodyTransformInfo* infos, int count)
        => _setBodiesTransform?.Invoke(infos, count);
        private static unsafe void SetCollidersTransform(ColliderTransformInfo* infos, int count)
        => _setCollidersTransform?.Invoke(infos, count);
        private static unsafe void SetBoxColliders(BoxColliderInfo* infos, int count)
        => _setBoxColliders?.Invoke(infos, count);
        private static unsafe void SetSphereColliders(SphereColliderInfo* infos, int count)
        => _setSphereColliders?.Invoke(infos, count);
        private static unsafe void SetCapsuleColliders(CapsuleColliderInfo* infos, int count)
        => _setCapsuleColliders?.Invoke(infos, count);
        #endregion

        #region 刚体加力
        private static unsafe void AddForce(BodyForceInfo* infos, int count)
        => _addForce?.Invoke(infos, count);
        private static unsafe void AddTorque(BodyTorqueInfo* infos, int count)
        => _addTorque?.Invoke(infos, count);
        #endregion
#else
        [DllImport(PLUGIN_NAME, EntryPoint = "StartEngine")]
        private extern static void StartEngine(int subStep, int iteration);
        [DllImport(PLUGIN_NAME, EntryPoint = "StopEngine")]
        private extern static void StopEngine();
        [DllImport(PLUGIN_NAME, EntryPoint = "EngineStep")]
        private extern static void EngineStep(float dt);
        [DllImport(PLUGIN_NAME, EntryPoint = "GetCollisionCount")]
        private extern static int GetCollisionCount();
        [DllImport(PLUGIN_NAME, EntryPoint = "GetCollisionPairs")]
        private extern static unsafe void GetCollisionPairs(CollisionPair* pairs, ref int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "GetDirtyBodyCount")]
        private extern static int GetDirtyBodyCount();
        [DllImport(PLUGIN_NAME, EntryPoint = "GetDirtyBody")]
        private extern static unsafe void GetDirtyBody(BodyState* infos, ref int count);

        [DllImport(PLUGIN_NAME, EntryPoint = "CreateBody")]
        private extern static Handle CreateBody();
        [DllImport(PLUGIN_NAME, EntryPoint = "CreateCollider")]
        private extern static Handle CreateCollider(ColliderType type, Handle body);
        [DllImport(PLUGIN_NAME, EntryPoint = "DestroyBody")]
        private extern static void DestroyBody(Handle handle);
        [DllImport(PLUGIN_NAME, EntryPoint = "DestroyCollider")]
        private extern static void DestroyCollider(Handle handle);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetColliderBody")]
        private extern static void SetColliderBody(Handle handle, Handle body);

        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodies")]
        private extern static unsafe void SetBodies(BodyMassInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesDrag")]
        private extern static unsafe void SetBodiesDrag(BodyDragInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesVelocity")]
        private extern static unsafe void SetBodiesVelocity(BodyVelocityInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetColliders")]
        private extern static unsafe void SetColliders(ColliderMaterialInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBodiesTransform")]
        private extern static unsafe void SetBodiesTransform(BodyTransformInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetCollidersTransform")]
        private extern static unsafe void SetCollidersTransform(ColliderTransformInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetBoxColliders")]
        private extern static unsafe void SetBoxColliders(BoxColliderInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetSphereColliders")]
        private extern static unsafe void SetSphereColliders(SphereColliderInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "SetCapsuleColliders")]
        private extern static unsafe void SetCapsuleColliders(CapsuleColliderInfo* infos, int count);

        [DllImport(PLUGIN_NAME, EntryPoint = "AddForce")]
        private extern static unsafe void AddForce(BodyForceInfo* infos, int count);
        [DllImport(PLUGIN_NAME, EntryPoint = "AddTorque")]
        private extern static unsafe void AddTorque(BodyTorqueInfo* infos, int count);
#endif
    }
}
