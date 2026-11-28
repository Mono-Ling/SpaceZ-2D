using System.Collections.Generic;
using SpaceZ_2D.Framework.RunTime;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 托管封装层：组件登记、脏数据打包下发、步进与回写
    /// <para>分工：<see cref="PhySystemNative"/> 只做签名映射，本层才承载"帧循环 + 组件生命周期"语义。</para>
    /// <para>对外只出现托管类型（Handle / BodyState / RaycastHitInfo / List），不暴露类型擦除的 NativeArray。</para>
    /// <para>碰撞回调暂未实现：Enter / Stay / Exit 三张表不采集（原生侧不取即不产生压力，也不泄漏）。</para>
    /// </summary>
    public static partial class PhySystem
    {
        public const int SUB_STEP_COUNT = 6;
        public const int SOLVER_ITERATION = 8;

        /// <summary>默认碰撞层（与原生 LayerMask::Default 一致，即层 0）</summary>
        public const ulong DEFAULT_LAYER = 1UL;

        #region 登记表
        private static readonly Dictionary<Handle, Rigidbody2D_Z> _bodies = new();
        private static readonly Dictionary<Handle, Collider2D_Z> _colliders = new();
        /// <summary>刚体 -> 名下动态碰撞体，销毁刚体时逐个摘除托管登记</summary>
        private static readonly Dictionary<Handle, HashSet<Handle>> _dynamicColliders = new();

        private static readonly HashSet<INativeComponent> _nativeComponents = new();
        private static readonly List<INativeComponent> _componentBuffer = new();
        #endregion

        #region 脏数据缓冲
        // capacity 传 0：首次 Add 才真正向原生申请内存，避免静态字段初始化早于 DLL 加载
        private static readonly NativeList<BodyProperty> _dirtyBodiesProperty = new(0);
        private static readonly NativeList<BodyVelocity> _dirtyBodiesVelocity = new(0);
        private static readonly NativeList<ForceInfo> _dirtyBodyForces = new(0);
        private static readonly NativeList<TorqueInfo> _dirtyBodyTorques = new(0);

        private static readonly NativeList<ColliderProperty> _dirtyCollidersProperty = new(0);
        private static readonly NativeList<ColliderCollision> _dirtyCollidersCollision = new(0);

        private static readonly NativeList<TransformInfo> _dirtyBodiesTransform = new(0);
        private static readonly NativeList<TransformInfo> _dirtyCollidersTransform = new(0);

        private static readonly NativeList<LineLength> _dirtyLineColliders = new(0);
        private static readonly NativeList<BoxExtents> _dirtyBoxColliders = new(0);
        private static readonly NativeList<SphereRadius> _dirtySphereColliders = new(0);
        private static readonly NativeList<CapsuleSize> _dirtyCapsuleColliders = new(0);
        #endregion

        private static bool _running;

        /// <summary>原生引擎是否已启动</summary>
        public static bool Running => _running;

        [RuntimeStart(-90)]
        private static void InitManaged()
        {
            StartEngine(SUB_STEP_COUNT, SOLVER_ITERATION);
            _running = true;
            PhyLateUpdate.Add(Step);
        }

        [RuntimeEnd(90)]
        private static void DestroyManaged()
        {
            PhyLateUpdate.Remove(Step);
            _running = false;

            StopEngine();

            _bodies.Clear();
            _colliders.Clear();
            _dynamicColliders.Clear();
            _nativeComponents.Clear();

            DisposeDirtyBuffers();
        }

        #region 帧循环
        /// <summary>物理帧：组件同步 -> 脏数据下发 -> 原生步进 -> 回写脏刚体</summary>
        private static void Step()
        {
            if(!_running)
                return;

            ComponentUpdate();
            SyncDirtyData();
            Step(PhyTime.deltaTime);
            ApplyDirtyBodies();
        }

        /// <summary>遍历副本，避免组件在遍历中注册/注销导致集合被改</summary>
        private static void ComponentUpdate()
        {
            _componentBuffer.Clear();
            _componentBuffer.AddRange(_nativeComponents);
            foreach(var component in _componentBuffer)
                component?.ComponentUpdate();
        }

        /// <summary>把各组件攒下的写请求一次性交给原生批量接口</summary>
        /// <remarks>批量接口的签名含指针，方法组转换到 NativeWriteFunc&lt;T&gt; 需要 unsafe 上下文</remarks>
        private static unsafe void SyncDirtyData()
        {
            _dirtyBodiesProperty.Sync(SetBodiesProperty);
            _dirtyBodiesVelocity.Sync(SetBodiesVelocity);
            _dirtyBodyForces.Sync(AddForces);
            _dirtyBodyTorques.Sync(AddTorques);

            _dirtyCollidersProperty.Sync(SetCollidersProperty);
            _dirtyCollidersCollision.Sync(SetCollidersCollision);

            _dirtyBodiesTransform.Sync(SetBodiesTransform);
            _dirtyCollidersTransform.Sync(SetCollidersTransform);

            _dirtyLineColliders.Sync(SetLineColliders);
            _dirtyBoxColliders.Sync(SetBoxColliders);
            _dirtySphereColliders.Sync(SetSphereColliders);
            _dirtyCapsuleColliders.Sync(SetCapsuleColliders);
        }

        /// <summary>
        /// 取本帧脏刚体并回写到对应组件，随后立刻释放原生缓冲
        /// <para>原生返回的是 NativeArray（每帧新分配、调用方负责回收），本层在此消化掉，不外露。</para>
        /// </summary>
        private static void ApplyDirtyBodies()
        {
            NativeArray raw = DirtyBodies();
            if(!raw)
                return;
            try
            {
                NativeArray<BodyState> states = raw.ToNativeArray<BodyState>();
                for(int i = 0; i < states.array.size; i++)
                {
                    ref BodyState state = ref states[i];
                    if(TryGetBody(state.handle, out var body, false))
                        body.SetBodyInfo(state.position, state.rotation, state.linearVelocity, state.angularVelocity);
                }
            }
            finally
            {
                // struct，Dispose 只清本地副本的 ptr；原生侧 new[] 出来的内存已归还
                raw.Dispose();
            }
        }

        private static void DisposeDirtyBuffers()
        {
            _dirtyBodiesProperty.Dispose();
            _dirtyBodiesVelocity.Dispose();
            _dirtyBodyForces.Dispose();
            _dirtyBodyTorques.Dispose();
            _dirtyCollidersProperty.Dispose();
            _dirtyCollidersCollision.Dispose();
            _dirtyBodiesTransform.Dispose();
            _dirtyCollidersTransform.Dispose();
            _dirtyLineColliders.Dispose();
            _dirtyBoxColliders.Dispose();
            _dirtySphereColliders.Dispose();
            _dirtyCapsuleColliders.Dispose();
        }
        #endregion

        #region 登记查询
        private static bool TryGetBody(Handle handle, out Rigidbody2D_Z body, bool debug = true)
        {
            if(!handle || !_bodies.TryGetValue(handle, out body))
            {
                if(debug)
                    Debug.LogWarning($"【Beta 托管物理引擎】无效刚体句柄{handle}");
                body = null;
                return false;
            }
            return true;
        }

        private static bool TryGetCollider(Handle handle, out Collider2D_Z collider, bool debug = true)
        {
            if(!handle || !_colliders.TryGetValue(handle, out collider))
            {
                if(debug)
                    Debug.LogWarning($"【Beta 托管物理引擎】无效碰撞体句柄{handle}");
                collider = null;
                return false;
            }
            return true;
        }
        #endregion
    }
}
