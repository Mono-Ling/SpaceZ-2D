using System.Runtime.InteropServices;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 圆形碰撞体半径批量信息
    /// <para>对应原生 ExportObjInfo&lt;Collider, float&gt;。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct SphereRadius
    {
        public Handle handle;
        public float radius;
    }
}
