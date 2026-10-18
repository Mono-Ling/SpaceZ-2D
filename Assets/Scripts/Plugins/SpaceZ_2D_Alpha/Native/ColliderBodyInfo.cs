using System.Runtime.InteropServices;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 碰撞体所属刚体信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ColliderBodyInfo
    {
        public Handle handle;
        public Handle body;
    }
}
