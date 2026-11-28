using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 碰撞体摩擦与弹性批量项，<see cref="PhySystem.SetCollidersProperty"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Collider, ColliderProperty&gt;：handle@0 staticFriction@8 dynamicFriction@12 elasticity@16，共 20 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ColliderProperty
    {
        public Handle handle;
        public float staticFriction;
        public float dynamicFriction;
        /// <summary>恢复系数，原生侧钳到 [0, 1]</summary>
        public float elasticity;
    }
}
