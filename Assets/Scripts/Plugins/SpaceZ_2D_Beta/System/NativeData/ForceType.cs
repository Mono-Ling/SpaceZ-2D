namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 施力模式，对应原生 Core::ForceMode（底层 int）
    /// <para>故意不叫 ForceMode：UnityEngine 里有同名类型，跨命名空间 using 会出现 CS0104 歧义，
    /// 与 Exp_CCD 侧的 ForceType 保持同一命名。</para>
    /// </summary>
    public enum ForceType
    {
        /// <summary>冲量：瞬时改变速度</summary>
        Impulse = 0,
        /// <summary>持续力：本帧累积，步进结束后清空</summary>
        Sustain = 1,
    }
}
