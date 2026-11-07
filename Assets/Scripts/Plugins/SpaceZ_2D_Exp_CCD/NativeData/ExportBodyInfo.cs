using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    [StructLayout(LayoutKind.Sequential)]
    public struct ExportBodyInfo
    {
        public Handle handle;
        public Vector2 position;
        public float rotation;
        public Vector2 linearVelocity;
        public float angularVelocity;
    }
}