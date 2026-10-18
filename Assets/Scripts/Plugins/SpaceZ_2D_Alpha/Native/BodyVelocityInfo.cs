using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 刚体速度信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyVelocityInfo
    {
        public Handle handle;
        public Vector2 linearVelocity;
        public float angularVelocity;
    }
}
