using System.Runtime.InteropServices;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 刚体属性批量信息
    /// <para>对应原生 ExportObjInfo&lt;Rigidbody, BodyProperty&gt;（Handle 已内联展平）。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyProperty
    {
        public Handle handle;
        public float mass;
        public float inertia;
        /// <summary>原生 bool 为 1 字节，MarshalAs 不可省略</summary>
        [MarshalAs(UnmanagedType.U1)] public bool useGravity;
    }
}
