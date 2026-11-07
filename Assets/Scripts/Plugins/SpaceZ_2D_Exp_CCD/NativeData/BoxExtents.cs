using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 矩形碰撞体尺寸批量信息
    /// <para>对应原生 ExportObjInfo&lt;Collider, Vector2&gt;，extents 为半长。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BoxExtents
    {
        public Handle handle;
        public Vector2 extents;
    }
}
