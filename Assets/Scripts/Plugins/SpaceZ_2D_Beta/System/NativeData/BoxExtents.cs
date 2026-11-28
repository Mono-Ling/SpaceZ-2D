using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 盒碰撞体半长批量项，<see cref="PhySystem.SetBoxColliders"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Collider, Vector2&gt;：handle@0 extents@8，共 16 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BoxExtents
    {
        public Handle handle;
        /// <summary>各轴半径（半长），不是全尺寸</summary>
        public Vector2 extents;
    }
}
