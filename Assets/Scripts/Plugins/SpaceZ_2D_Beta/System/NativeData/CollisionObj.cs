using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 一对碰撞对象，对应原生 UnityPlugin::CollisionObj
    /// <para>共 4 个句柄、32 字节，顺序不可重排：firstCollider / firstBody / secondCollider / secondBody。</para>
    /// <para>未挂刚体的一侧，body 句柄为 id == -1 的空句柄。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CollisionObj
    {
        public Handle firstCollider;
        public Handle firstBody;

        public Handle secondCollider;
        public Handle secondBody;
    }
}
