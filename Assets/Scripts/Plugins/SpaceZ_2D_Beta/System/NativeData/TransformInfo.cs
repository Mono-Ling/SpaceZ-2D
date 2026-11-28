using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 变换批量项，SetBodiesTransform / SetCollidersTransform 共用
    /// <para>对应原生 ObjInfo&lt;Rigidbody, Transform&gt; 与 ObjInfo&lt;Collider, Transform&gt;：两者载荷布局完全相同，故共用一个结构。</para>
    /// <para>handle@0 position@8 rotation@16 scale@20，共 28 字节。字段顺序必须与原生 Transform 一致。</para>
    /// <para>刚体变换是世界变换；碰撞体变换是相对归属刚体的局部变换，无归属时即世界变换。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TransformInfo
    {
        public Handle handle;
        public Vector2 position;
        /// <summary>弧度</summary>
        public float rotation;
        /// <summary>物理里 scale 只有"大小"语义，须非负；非均匀缩放是合法输入</summary>
        public Vector2 scale;

        /// <summary>
        /// 与原生默认构造一致的初值（scale = 1）
        /// <para>注意：C# 的 default(TransformInfo) 会把 scale 置 0，直接传给原生会让碰撞体退化成零尺寸。</para>
        /// </summary>
        public static TransformInfo Default => new() { scale = Vector2.one };
    }
}
