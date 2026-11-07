using System.Runtime.InteropServices;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 刚体阻尼批量信息
    /// <para>对应原生 ExportObjInfo&lt;Rigidbody, std::pair&lt;float, float&gt;&gt;。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyDrag
    {
        public Handle handle;
        public float linearDrag;
        public float angularDrag;
    }
}
