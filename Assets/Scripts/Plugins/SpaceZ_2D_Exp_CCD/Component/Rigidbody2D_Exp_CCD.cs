using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 刚体组件：属性映射、Unity 生命周期与变换同步。
    /// <para>句柄登记、销毁与批量下发由基类 <see cref="PhySystem.Rigidbody2D_Exp_CCD"/> 提供。</para>
    /// </summary>
    public class Rigidbody2D_Exp_CCD : PhySystem.Rigidbody2D_Exp_CCD
    {
        #region 速度
        /// <summary>线速度。每物理帧由原生侧回写，赋值立即下发</summary>
        public Vector2 LinearVelocity
        {
            get => _linearVelocity;
            set => SetVelocity(value, _angularVelocity);
        }
        /// <summary>角速度（弧度/秒）。每物理帧由原生侧回写，赋值立即下发</summary>
        public float AngularVelocity
        {
            get => _angularVelocity;
            set => SetVelocity(_linearVelocity, value);
        }
        #endregion

        #region 质量属性
        public float Mass
        {
            get => _mass;
            set
            {
                value = Mathf.Max(0f, value);
                if(value == _mass)
                    return;
                _mass = value;
                SetBodyProperty(_mass, _inertia, _useGravity);
            }
        }
        public float Inertia
        {
            get => _inertia;
            set
            {
                value = Mathf.Max(0f, value);
                if(value == _inertia)
                    return;
                _inertia = value;
                SetBodyProperty(_mass, _inertia, _useGravity);
            }
        }
        public bool UseGravity
        {
            get => _useGravity;
            set
            {
                if(value == _useGravity)
                    return;
                _useGravity = value;
                SetBodyProperty(_mass, _inertia, _useGravity);
            }
        }
        public float LinearDrag
        {
            get => _linearDrag;
            set
            {
                value = Mathf.Clamp01(value);
                if(value == _linearDrag)
                    return;
                _linearDrag = value;
                SetBodyDrag(_linearDrag, _angularDrag);
            }
        }
        public float AngularDrag
        {
            get => _angularDrag;
            set
            {
                value = Mathf.Clamp01(value);
                if(value == _angularDrag)
                    return;
                _angularDrag = value;
                SetBodyDrag(_linearDrag, _angularDrag);
            }
        }
        #endregion

        [SerializeField] private float _mass = 1.0f;
        [SerializeField] private float _inertia = 1.0f;
        [SerializeField] private bool _useGravity = true;
        [SerializeField] private float _linearDrag = 0.0f;
        [SerializeField] private float _angularDrag = 0.05f;

        public override void ComponentUpdate() => SyncTransform();

        private void OnEnable()
        {
            CreateBody();
            if(!_handle)
                return;

            // 初始配置没有"旧值"可比，必须无条件下发
            SetBodyProperty(_mass, _inertia, _useGravity);
            SetBodyDrag(_linearDrag, _angularDrag);

            // 原生刚体从原点起步，首次创建与重建都必须把当前变换下发一次
            _transformSynced = false;
            SyncTransform();
        }
        private void OnDisable() => DestroyBody();

        /// <summary>比较 Unity 变换与上次同步值，变化时下发到原生侧</summary>
        private void SyncTransform()
        {
            if(!_handle)
                return;

            Vector2 pos = transform.position;
            float rot = transform.eulerAngles.z * Mathf.Deg2Rad;
            Vector2 scale = transform.lossyScale;

            if(_transformSynced
            && pos == _preTransform.pos
            && rot == _preTransform.rot
            && scale == _preTransform.scale)
                return;

            _preTransform = (pos, rot, scale);
            _transformSynced = true;
            SetBodyTransform(pos, rot, scale);
        }
    }
}
