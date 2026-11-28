using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 碰撞体层与碰撞模式批量项，<see cref="PhySystem.SetCollidersCollision"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Collider, ColliderCollision&gt;：handle@0 layer@8 mode@16，共 24 字节（尾部 4 字节填充）。</para>
    /// <para>layer 直接传 64 位掩码，不是 LayerMask 结构体；传 0 表示不与任何层碰撞。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ColliderCollision
    {
        public Handle handle;
        public ulong layer;
        public CollisionMode mode;
    }
}
