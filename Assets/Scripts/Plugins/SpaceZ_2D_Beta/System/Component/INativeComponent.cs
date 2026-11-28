namespace SpaceZ_2D.Beta
{
    public static partial class PhySystem
    {
        /// <summary>
        /// 托管组件在物理帧里被回调的统一入口
        /// </summary>
        public interface INativeComponent
        {
            Handle Handle { get; }
            /// <summary>每个物理帧在原生步进之前调用，用于把 Unity 侧状态攒进脏数据缓冲</summary>
            void ComponentUpdate();
        }
    }
}
