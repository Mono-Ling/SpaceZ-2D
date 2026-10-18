using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyState
    {
        public Handle handle;
        public float rotation;
        public float angularVelocity;
        public Vector2 position;
        public Vector2 linearVelocity;
    }
}