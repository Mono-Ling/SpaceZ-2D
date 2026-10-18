using System.Runtime.InteropServices;

namespace SpaceZ_2D.Alpha
{
    /// <summary>
    /// 碰撞体材质信息(静摩擦、动摩擦、弹性)
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ColliderMaterialInfo
    {
        public Handle handle;
        public float staticFriction;
        public float dynamicFriction;
        public float elasticity;
    }
}
