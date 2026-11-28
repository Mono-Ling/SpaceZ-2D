using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>盒碰撞体，Extents 为各轴半径（半长）</summary>
    public class BoxCollider2D_Z : Collider2D_Z
    {
        public Vector2 Extents
        {
            get => _extents;
            set
            {
                value = new() { x = Mathf.Abs(value.x), y = Mathf.Abs(value.y) };
                if(value == _extents)
                    return;
                _extents = value;
                SetBox(_extents);
            }
        }

        [SerializeField] private Vector2 _extents = new(0.5f, 0.5f);

        private void OnEnable()
        {
            Create(ColliderType.Box);
            SetBox(_extents);
        }

        private void OnDrawGizmosSelected()
        {
            Gizmos.color = Color.green;
            Gizmos.matrix = transform.localToWorldMatrix;
            Gizmos.DrawWireCube(Vector3.zero, _extents * 2);
            Gizmos.matrix = Matrix4x4.identity;
        }
    }
}
