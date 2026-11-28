using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 球碰撞体半径批量项，<see cref="PhySystem.SetSphereColliders"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Collider, float&gt;：handle@0 radius@8，共 12 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct SphereRadius
    {
        public Handle handle;
        public float radius;
    }
}
