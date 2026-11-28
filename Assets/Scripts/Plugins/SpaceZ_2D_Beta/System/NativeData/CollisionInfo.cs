using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 单个接触点信息，对应原生 UnityPlugin::CollisionInfo
    /// <para>depth@0 point@4 normal@12，共 20 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CollisionInfo
    {
        /// <summary>穿透深度</summary>
        public float depth;
        /// <summary>接触点（世界坐标）</summary>
        public Vector2 point;
        /// <summary>接触法向（由 first 指向 second）</summary>
        public Vector2 normal;
    }
}
