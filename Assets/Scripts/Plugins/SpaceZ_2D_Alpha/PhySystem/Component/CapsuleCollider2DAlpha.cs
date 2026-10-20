using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public class CapsuleCollider2DAlpha : PhySystem.Collider2DAlpha
    {
        public float Height
        {
            get => _height;
            set
            {
                value = Mathf.Abs(value);
                if(value == _height)
                    return;
                SetCapsule(value, _radius);
                _height = value;
            }
        }
        public float Radius
        {
            get => _height;
            set
            {
                value = Mathf.Abs(value);
                if(value == _radius)
                    return;
                SetCapsule(_height, value);
                _radius = value;
            }
        }
        [SerializeField]
        private float _height = 1f;
        [SerializeField]
        private float _radius = 0.5f;
        void OnEnable()
        {
            Create(ColliderType.Capsule);
            SetCapsule(_height, _radius);
        }
    }
}