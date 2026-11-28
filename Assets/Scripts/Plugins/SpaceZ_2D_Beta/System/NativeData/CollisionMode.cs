namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 碰撞检测模式（对应原生 Core::CollisionMode，底层 int）
    /// </summary>
    public enum CollisionMode
    {
        Discrete = 0,
        /// <summary>连续检测，需碰撞体挂在刚体上才生效，否则退化为 Discrete</summary>
        Continuous = 1,
    }
}
