using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 刚体线性力批量项，<see cref="PhySystem.AddForces"/> 的元素
    /// <para>对应原生 ObjInfo&lt;Rigidbody, ForceInfo&gt;：handle@0 force@8 mode@16，共 20 字节。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ForceInfo
    {
        public Handle handle;
        public Vector2 force;
        public ForceType mode;
    }
}
