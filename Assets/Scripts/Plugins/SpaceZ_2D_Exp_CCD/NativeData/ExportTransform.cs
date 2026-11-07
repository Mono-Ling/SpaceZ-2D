using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 变换批量项（刚体 / 碰撞体共用，原生侧同一个 ExportTransform）
    /// <para>对应原生 ExportObjInfo&lt;Rigidbody, ExportTransform&gt; 与
    /// ExportObjInfo&lt;Collider, ExportTransform&gt;：两者载荷布局完全相同，故共用一个结构。</para>
    /// <para>字段顺序必须与 ExportTransform.h 一致：position → rotation → scale。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ExportTransform
    {
        public Handle handle;
        public Vector2 position;
        public float rotation;
        public Vector2 scale;
    }
}
