namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 碰撞体形状（对应原生 Core::ColliderType，底层 int）
    /// </summary>
    public enum ColliderType
    {
        Line = 0,
        Box = 1,
        Sphere = 2,
        Capsule = 3,
    }
}
