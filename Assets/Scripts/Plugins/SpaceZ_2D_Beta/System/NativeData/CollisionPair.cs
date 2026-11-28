using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 碰撞对及其接触点，EnterCollisionPairs / StayCollisionPairs 的元素
    /// <para>对应原生 UnityPlugin::CollisionPair：collision@0（32 字节） infos@32（ptr + size，16 字节），共 48 字节。</para>
    /// <para><b>infos 是原生侧单独分配的二级缓冲</b>：外层 NativeArray 与每个元素的 infos 都要各自 Dispose，
    /// 只释放外层会漏掉全部接触点缓冲。映射层不代劳，由调用方按需释放。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CollisionPair
    {
        public CollisionObj collision;
        /// <summary>NativeArray&lt;CollisionInfo&gt;，size 是元素个数（不是字节数），需单独 Dispose</summary>
        public NativeArray infos;
    }
}
