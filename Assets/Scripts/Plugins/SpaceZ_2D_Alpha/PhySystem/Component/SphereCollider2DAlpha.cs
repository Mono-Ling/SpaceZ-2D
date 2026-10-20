using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public class SphereCollider2DAlpha : PhySystem.Collider2DAlpha
    {
        public float Radius
        {
            get => _radius;
            set
            {
                value = Mathf.Abs(value);
                if(value == _radius)
                    return;
                SetSphere(value);
                _radius = value;
            }
        }
        [SerializeField]
        private float _radius = 0.5f;
        void OnEnable()
        {
            Create(ColliderType.Sphere);
            SetSphere(_radius);
        }
        void OnDrawGizmosSelected()
        {
            Gizmos.color = Color.green;
            Gizmos.matrix = transform.localToWorldMatrix;
            Gizmos.DrawWireSphere(Vector3.zero, _radius);
            Gizmos.matrix = Matrix4x4.identity;
        }
    }
}