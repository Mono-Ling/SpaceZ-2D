using System.Runtime.InteropServices;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 胶囊碰撞体尺寸批量信息
    /// <para>对应原生 ExportObjInfo&lt;Collider, std::pair&lt;float, float&gt;&gt;。</para>
    /// <para>字段顺序必须与 SetCapsuleCollider(handle, r, h) 一致：radius → height。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CapsuleSize
    {
        public Handle handle;
        public float radius;
        public float height;
    }
}
