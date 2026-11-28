using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 刚体速度批量项，<see cref="PhySystem.SetBodiesVelocity"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Rigidbody, BodyVelocity&gt;：handle@0 linearVelocity@8 angularVelocity@16，共 20 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyVelocity
    {
        public Handle handle;
        public Vector2 linearVelocity;
        /// <summary>角速度（弧度/秒）</summary>
        public float angularVelocity;
    }
}
