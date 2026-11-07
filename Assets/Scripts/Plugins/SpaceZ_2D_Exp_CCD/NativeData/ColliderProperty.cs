using System.Runtime.InteropServices;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 碰撞体材质批量信息（静摩擦、动摩擦、弹性）
    /// <para>对应原生 ExportObjInfo&lt;Collider, ColliderProperty&gt;。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ColliderProperty
    {
        public Handle handle;
        public float staticFriction;
        public float dynamicFriction;
        public float elasticity;
    }
}
