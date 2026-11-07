using System.Runtime.InteropServices;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 刚体速度批量信息
    /// <para>对应原生 ExportObjInfo&lt;Rigidbody, std::pair&lt;Vector2, float&gt;&gt;。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct BodyVelocity
    {
        public Handle handle;
        public Vector2 linearVelocity;
        public float angularVelocity;
    }
}
