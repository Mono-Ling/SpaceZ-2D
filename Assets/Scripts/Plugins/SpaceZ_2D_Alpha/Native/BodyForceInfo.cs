using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 刚体线性力信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyForceInfo
    {
        public Handle handle;
        public Vector2 force;
    }
}
