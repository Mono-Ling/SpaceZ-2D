using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 线段长度批量项，<see cref="PhySystem.SetLineColliders"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Collider, float&gt;：handle@0 length@8，共 12 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct LineLength
    {
        public Handle handle;
        /// <summary>线段全长（不是半长）</summary>
        public float length;
    }
}
