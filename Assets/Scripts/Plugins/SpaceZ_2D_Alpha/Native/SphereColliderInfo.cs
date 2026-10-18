using System.Runtime.InteropServices;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 圆形碰撞体半径信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct SphereColliderInfo
    {
        public Handle handle;
        public float radius;
    }
}
