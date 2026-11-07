namespace SpaceZ_2D.Exp.CCD
{
    /// <summary>
    /// 刚体施力类型（对应原生 Core::ForceType，底层为 int）
    /// </summary>
    public enum ForceType
    {
        /// <summary>冲量：瞬时改变速度，受质量影响</summary>
        Impulse = 0,
        /// <summary>持续力：本帧内持续作用，步进结束后清空</summary>
        Sustain = 1,
    }
}
