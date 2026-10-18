using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CollisionPair
    {
        public Handle firstCollider;
        public Handle firstBody;

        public Handle secondCollider;
        public Handle secondBody;

        public float depth;
        public Vector2 point;
        public Vector2 normal;
    }
}