using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 碰撞体组件：属性映射、Unity 生命周期与"相对归属刚体"的变换解析。
    /// <para>句柄登记、归属切换与批量下发由基类 <see cref="PhySystem.Collider2D_Exp_CCD"/> 提供。</para>
    /// </summary>
    public abstract class Collider2D_Exp_CCD : PhySystem.Collider2D_Exp_CCD
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
        /// <summary>弹性系数，原生侧按 [0, 1] 截断</summary>
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

        [SerializeField] private float _staticFriction = 0.0f;
        [SerializeField] private float _dynamicFriction = 0.0f;
        [SerializeField] private float _elasticity = 0.2f;

        /// <summary>归属刚体组件，静态碰撞体为 null</summary>
        private Rigidbody2D_Exp_CCD _bodyComp;
        /// <summary>归属刚体的 Transform，静态碰撞体为 null</summary>
        private Transform _bodyObj;
        /// <summary>上一次下发的局部变换，用于抑制无变化帧的重复下发</summary>
        private (Vector2 pos, float rot, Vector2 scale) _preTransform;
        private bool _transformSynced;

        public override void ComponentUpdate() => SyncTransform();

        /// <summary>绑定归属刚体并创建原生碰撞体（由子类 OnEnable 调用）</summary>
        protected void Create(ColliderType type)
        {
            _bodyComp = GetComponentInParent<Rigidbody2D_Exp_CCD>();
            _bodyObj = _bodyComp != null ? _bodyComp.transform : null;
            _body = _bodyComp != null ? _bodyComp.Handle : Handle.NULL;

            CreateCollider(type);
            // 初始材质没有"旧值"可比，必须无条件下发
            SetColliderProperty(_staticFriction, _dynamicFriction, _elasticity);
            ForceSyncTransform();
        }

        protected virtual void OnDisable() => DestroyCollider();
        protected virtual void OnDestroy() => DestroyCollider();

        protected virtual void OnTransformParentChanged()
        {
            if(!_handle)
                return;

            var comp = GetComponentInParent<Rigidbody2D_Exp_CCD>();
            var obj = comp != null ? comp.transform : null;
            if(obj == _bodyObj)
                return;

            _bodyComp = comp;
            _bodyObj = obj;
            // SetBody 依赖 _body 的旧值做差异判断，不能先改 _body
            SetBody(comp != null ? comp.Handle : Handle.NULL);
            ForceSyncTransform();
        }

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
            // 原生 LocalScale 取绝对值，比较口径需一致
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
