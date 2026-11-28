using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 刚体物理属性批量项，<see cref="PhySystem.SetBodiesProperty"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Rigidbody, BodyProperty&gt;（句柄已展平到首字段）。布局：</para>
    /// <para>handle@0 mass@8 inertia@12 useGravity@16(1 字节 + 3 字节填充) linearDrag@20 angularDrag@24，共 28 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyProperty
    {
        public Handle handle;
        public float mass;
        public float inertia;
        /// <summary>原生 bool 是 1 字节，MarshalAs 不可省略；默认 4 字节 BOOL 会把后面两个 float 顶偏</summary>
        [MarshalAs(UnmanagedType.U1)] public bool useGravity;
        public float linearDrag;
        public float angularDrag;
    }
}
