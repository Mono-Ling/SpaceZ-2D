using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 刚体力矩批量项，<see cref="PhySystem.AddTorques"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Rigidbody, TorqueInfo&gt;：handle@0 torque@8 mode@12，共 16 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TorqueInfo
    {
        public Handle handle;
        public float torque;
        public ForceType mode;
    }
}
