using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 刚体线性力批量信息
    /// <para>对应原生 ExportObjInfo&lt;Rigidbody, std::pair&lt;Vector2, ForceType&gt;&gt;。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyForce
    {
        public Handle handle;
        public Vector2 force;
        public ForceType type;
    }
}
