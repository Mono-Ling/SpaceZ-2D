using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyDragInfo
    {
        public Handle handle;
        public float linearDrag;
        public float angularDrag;
    }
}