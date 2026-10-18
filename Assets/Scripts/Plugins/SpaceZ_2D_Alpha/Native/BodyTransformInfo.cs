using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 刚体变换信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyTransformInfo
    {
        public Handle handle;
        public float rotation;
        public Vector2 position;
        public Vector2 scale;
    }
}
