using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>矩形碰撞体，Extents 为半长</summary>
    public class BoxCollider2D_Exp_CCD : Collider2D_Exp_CCD
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
        [SerializeField] private Vector2 _extents = Vector2.one;

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
