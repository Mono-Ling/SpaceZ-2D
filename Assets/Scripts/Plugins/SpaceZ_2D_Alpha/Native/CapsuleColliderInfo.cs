using System.Runtime.InteropServices;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 胶囊碰撞体尺寸信息(高度、半径)
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CapsuleColliderInfo
    {
        public Handle handle;
        public float height;
        public float radius;
    }
}
