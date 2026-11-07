using System.Runtime.InteropServices;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 刚体力矩批量信息
    /// <para>对应原生 ExportObjInfo&lt;Rigidbody, std::pair&lt;float, ForceType&gt;&gt;。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyTorque
    {
        public Handle handle;
        public float torque;
        public ForceType type;
    }
}
