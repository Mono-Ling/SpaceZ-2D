using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 线段碰撞体（无厚度）。
    /// <para>Length 为全长，线段沿局部 X 轴、中心在原点，端点分别为 (-Length/2, 0) 与 (Length/2, 0)。</para>
    /// <para>只作为静态碰撞体会更有意义；挂到刚体上时按普通凸体参与求解。</para>
    /// </summary>
    public class LineCollider2D_Z : Collider2D_Z
    {
        public float Length
        {
            get => _length;
            set
            {
                value = Mathf.Abs(value);
                if(value == _length)
                    return;
                _length = value;
                SetLine(_length);
            }
        }

        [SerializeField] private float _length = 1.0f;

        private void OnEnable()
        {
            Create(ColliderType.Line);
            SetLine(_length);
        }

        private void OnDrawGizmosSelected()
        {
            Gizmos.color = Color.green;
            Gizmos.matrix = transform.localToWorldMatrix;

            float half = _length * 0.5f;
            Gizmos.DrawLine(new Vector3(-half, 0.0f, 0.0f), new Vector3(half, 0.0f, 0.0f));
            Gizmos.DrawWireSphere(new Vector3(-half, 0.0f, 0.0f), 0.02f);
            Gizmos.DrawWireSphere(new Vector3(half, 0.0f, 0.0f), 0.02f);

            Gizmos.matrix = Matrix4x4.identity;
        }
    }
}
