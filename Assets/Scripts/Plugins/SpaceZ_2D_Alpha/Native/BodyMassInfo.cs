using System.Runtime.InteropServices;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 刚体质量信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyMassInfo
    {
        public Handle handle;
        public float mass;
        public float inertia;
    }
}
