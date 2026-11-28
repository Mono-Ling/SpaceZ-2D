using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 脏刚体状态，<see cref="PhySystem.DirtyBodies"/> 的元素
    /// <para>对应原生 UnityPlugin::BodyState，字段顺序不可重排：</para>
    /// <para>handle@0（8 字节） position@8 rotation@16 linearVelocity@20 angularVelocity@28，共 32 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyState
    {
        public Handle handle;
        public Vector2 position;
        /// <summary>弧度</summary>
        public float rotation;
        public Vector2 linearVelocity;
        public float angularVelocity;
    }
}
