using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>球碰撞体，Radius 为半径</summary>
    public class SphereCollider2D_Z : Collider2D_Z
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
                SetSphere(_radius);
            }
        }

        [SerializeField] private float _radius = 0.5f;

        private void OnEnable()
        {
            Create(ColliderType.Sphere);
            SetSphere(_radius);
        }

        private void OnDrawGizmosSelected()
        {
            Gizmos.color = Color.green;
            Gizmos.matrix = transform.localToWorldMatrix;
            Gizmos.DrawWireSphere(Vector3.zero, _radius);
            Gizmos.matrix = Matrix4x4.identity;
        }
    }
}
