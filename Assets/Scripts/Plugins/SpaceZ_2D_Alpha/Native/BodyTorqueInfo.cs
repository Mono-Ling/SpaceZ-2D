using System.Runtime.InteropServices;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 刚体力矩信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyTorqueInfo
    {
        public Handle handle;
        public float torque;
    }
}
