using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public class BoxCollider2DAlpha : PhySystem.Collider2DAlpha
    {
        public Vector2 Extents
        {
            get => _extents;
            set
            {
                value = new() { x = Mathf.Abs(value.x), y = Mathf.Abs(value.y) };
                if(value == _extents)
                    return;
                SetBox(value);
                _extents = value;
            }
        }
        [SerializeField]
        private Vector2 _extents = Vector2.one;

        void OnEnable()
        {
            Create(ColliderType.Box);
            SetBox(_extents);
        }
        void OnDrawGizmosSelected()
        {
            Gizmos.color = Color.green;
            Gizmos.matrix = transform.localToWorldMatrix;
            Gizmos.DrawWireCube(Vector3.zero, _extents * 2);
            Gizmos.matrix = Matrix4x4.identity;
        }
    }
}