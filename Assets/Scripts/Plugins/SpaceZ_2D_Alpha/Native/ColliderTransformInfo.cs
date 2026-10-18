using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 碰撞体局部变换信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ColliderTransformInfo
    {
        public Handle handle;
        public float rotation;
        public Vector2 position;
        public Vector2 scale;
    }
}
