using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 射线命中结果，对应原生 UnityPlugin::RaycastHit
    /// <para>collider@0（8 字节） point@8 normal@16 distance@24，共 28 字节。</para>
    /// <para>故意不叫 RaycastHit：UnityEngine 里有同名类型，跨命名空间 using 会出现 CS0104 歧义。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct RaycastHitInfo
    {
        public Handle collider;
        /// <summary>命中点（世界坐标）</summary>
        public Vector2 point;
        /// <summary>命中面法向</summary>
        public Vector2 normal;
        /// <summary>命中距离；-1 表示未命中（与原生口径一致）</summary>
        public float distance;
    }
}
