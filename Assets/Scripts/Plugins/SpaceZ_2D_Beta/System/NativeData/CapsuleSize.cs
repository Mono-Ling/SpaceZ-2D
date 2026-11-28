using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 胶囊碰撞体尺寸批量项，<see cref="PhySystem.SetCapsuleColliders"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Collider, CapsuleSize&gt;：handle@0 radius@8 height@12，共 16 字节。</para>
    /// <para>字段顺序必须与 SetCapsuleCollider(handle, r, h) 一致：radius → height。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CapsuleSize
    {
        public Handle handle;
        public float radius;
        /// <summary>圆柱段高度，不含两端半球</summary>
        public float height;
    }
}
