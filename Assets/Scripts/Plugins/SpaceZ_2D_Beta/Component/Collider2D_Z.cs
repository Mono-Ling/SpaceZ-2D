using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 碰撞体组件：材质/层/碰撞模式映射、Unity 生命周期与"相对归属刚体"的变换解析
    /// <para>句柄登记、归属切换与批量下发由基类 <see cref="PhySystem.Collider2D_Z"/> 提供。</para>
    /// </summary>
    public abstract class Collider2D_Z : PhySystem.Collider2D_Z
    {
        #region 材质属性
        public float StaticFriction
        {
            get => _staticFriction;
            set
            {
                value = Mathf.Max(0f, value);
                if(value == _staticFriction)
                    return;
                _staticFriction = value;
                SetColliderProperty(_staticFriction, _dynamicFriction, _elasticity);
            }
        }

        public float DynamicFriction
        {
            get => _dynamicFriction;
            set
            {
                value = Mathf.Max(0f, value);
                if(value == _dynamicFriction)
                    return;
                _dynamicFriction = value;
                SetColliderProperty(_staticFriction, _dynamicFriction, _elasticity);
            }
        }

        /// <summary>恢复系数，原生侧按 [0, 1] 截断</summary>
        public float Elasticity
        {
            get => _elasticity;
            set
            {
                value = Mathf.Clamp01(value);
                if(value == _elasticity)
                    return;
                _elasticity = value;
                SetColliderProperty(_staticFriction, _dynamicFriction, _elasticity);
            }
        }
        #endregion

        #region 层与碰撞模式
        /// <summary>
        /// 碰撞层掩码（64 位，位 n 表示层 n）。原生默认只有层 0，即 1。
        /// </summary>
        public ulong Layer
        {
            get => _layer;
            set
            {
                if(value == _layer)
                    return;
                _layer = value;
                SetColliderCollision(_layer, _collisionMode);
            }
        }

        /// <summary>
        /// 碰撞检测模式。Continuous 需碰撞体挂在刚体上才生效，否则退化为 Discrete。
        /// </summary>
        public CollisionMode Mode
        {
            get => _collisionMode;
            set
            {
                if(value == _collisionMode)
                    return;
                _collisionMode = value;
                SetColliderCollision(_layer, _collisionMode);
            }
        }
        #endregion

        [SerializeField] private float _staticFriction = 0.6f;
        [SerializeField] private float _dynamicFriction = 0.4f;
        [SerializeField] private float _elasticity = 0f;
        [SerializeField] private ulong _layer = PhySystem.DEFAULT_LAYER;
        [SerializeField] private CollisionMode _collisionMode = CollisionMode.Discrete;

        /// <summary>归属刚体组件，静态碰撞体为 null</summary>
        private Rigidbody2D_Z _bodyComp;
        /// <summary>归属刚体的 Transform，静态碰撞体为 null</summary>
        private Transform _bodyObj;

        /// <summary>上一次下发的局部变换，用于抑制无变化帧的重复下发</summary>
        private (Vector2 pos, float rot, Vector2 scale) _preTransform;
        private bool _transformSynced;

        public override void ComponentUpdate() => SyncTransform();

        /// <summary>绑定归属刚体并创建原生碰撞体（由子类 OnEnable 调用）</summary>
        protected void Create(ColliderType type)
        {
            _bodyComp = GetComponentInParent<Rigidbody2D_Z>();
            _bodyObj = _bodyComp != null ? _bodyComp.transform : null;
            _body = _bodyComp != null ? _bodyComp.Handle : Handle.NULL;

            CreateCollider(type);

            // 初始配置没有"旧值"可比，必须无条件下发
            SetColliderProperty(_staticFriction, _dynamicFriction, _elasticity);
            SetColliderCollision(_layer, _collisionMode);
            ForceSyncTransform();
        }

        protected virtual void OnDisable() => DestroyCollider();
        protected virtual void OnDestroy() => DestroyCollider();

        /// <summary>
        /// 重新解析归属刚体并同步归属（父级变化、或刚体晚于本碰撞体创建时由刚体侧调用）
        /// <para>没有原生句柄时直接返回：未创建/未激活的碰撞体由自己的 <see cref="Create"/> 负责首次绑定。</para>
        /// <para>判据必须带上"期望归属句柄是否已生效"，不能只看 Transform —— 碰撞体先于刚体创建时
        /// Transform 已经一致，但 _body 仍是空句柄。</para>
        /// </summary>
        public void SyncBody()
        {
            if(!_handle)
                return;

            var comp = GetComponentInParent<Rigidbody2D_Z>();
            var obj = comp != null ? comp.transform : null;
            Handle expected = comp != null ? comp.Handle : Handle.NULL;

            if(obj == _bodyObj && comp == _bodyComp && _body == expected)
                return;

            _bodyComp = comp;
            _bodyObj = obj;
            // SetBody 依赖 _body 的旧值做差异判断，不能先改 _body
            SetBody(expected);
            ForceSyncTransform();
        }

        protected virtual void OnTransformParentChanged() => SyncBody();

        /// <summary>本地变换为相对归属刚体的局部变换；无刚体时即世界变换</summary>
        private void SyncTransform()
        {
            if(!_handle)
                return;

            // 刚体的 OnEnable 可能晚于本组件，句柄在其建好后才可用；此处补齐绑定
            if(_bodyComp != null && _body != _bodyComp.Handle)
                SetBody(_bodyComp.Handle);

            Matrix4x4 local = (_bodyObj != null ? _bodyObj.worldToLocalMatrix : Matrix4x4.identity)
                            * transform.localToWorldMatrix;
            Vector3 pos = local.GetPosition();
            Vector3 scale = local.lossyScale;
            Vector2 currPos = pos;
            // 原生 scale 只有"大小"语义，比较口径取绝对值
            Vector2 currScale = new() { x = Mathf.Abs(scale.x), y = Mathf.Abs(scale.y) };
            float currRot = local.rotation.eulerAngles.z * Mathf.Deg2Rad;

            if(_transformSynced
            && currPos == _preTransform.pos
            && currRot == _preTransform.rot
            && currScale == _preTransform.scale)
                return;

            _preTransform = (currPos, currRot, currScale);
            _transformSynced = true;
            SetColliderTransform(currPos, currRot, currScale);
        }

        private void ForceSyncTransform()
        {
            _transformSynced = false;
            SyncTransform();
        }
    }
}
