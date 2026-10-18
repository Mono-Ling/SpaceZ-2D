using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 矩形碰撞体尺寸信息(半长)
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BoxColliderInfo
    {
        public Handle handle;
        public Vector2 extents;
    }
}
