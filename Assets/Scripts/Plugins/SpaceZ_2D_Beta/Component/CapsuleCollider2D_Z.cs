using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 胶囊碰撞体。
    /// <para>Height 为两端球心间距（圆柱段长度，不含半球），整体高度为 Height + 2 * Radius。</para>
    /// <para>胶囊轴为局部 Y。</para>
    /// </summary>
    public class CapsuleCollider2D_Z : Collider2D_Z
    {
        public float Radius
        {
            get => _radius;
            set
            {
                value = Mathf.Abs(value);
                if(value == _radius)
                    return;
                _radius = value;
                SetCapsule(_radius, _height);
            }
        }

        public float Height
        {
            get => _height;
            set
            {
                value = Mathf.Abs(value);
                if(value == _height)
                    return;
                _height = value;
                SetCapsule(_radius, _height);
            }
        }

        [SerializeField] private float _radius = 0.5f;
        [SerializeField] private float _height = 1.0f;

        private void OnEnable()
        {
            Create(ColliderType.Capsule);
            SetCapsule(_radius, _height);
        }

        private void OnDrawGizmosSelected()
        {
            Gizmos.color = Color.green;
            Gizmos.matrix = transform.localToWorldMatrix;

            float halfH = _height * 0.5f;
            Vector3 top = new(0.0f, halfH, 0.0f);
            Vector3 bottom = new(0.0f, -halfH, 0.0f);
            Gizmos.DrawLine(top + Vector3.right * _radius, bottom + Vector3.right * _radius);
            Gizmos.DrawLine(top + Vector3.left * _radius, bottom + Vector3.left * _radius);
            Gizmos.DrawWireSphere(top, _radius);
            Gizmos.DrawWireSphere(bottom, _radius);

            Gizmos.matrix = Matrix4x4.identity;
        }
    }
}
