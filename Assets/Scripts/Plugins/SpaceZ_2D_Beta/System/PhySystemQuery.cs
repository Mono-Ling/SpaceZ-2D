using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 空间查询的托管出口：把原生返回的 NativeArray 就地消费成托管集合
    /// <para>对外不出现 <see cref="NativeArray"/>，调用方拿到的是 List / out 参数，内存由本层负责回收。</para>
    /// </summary>
    public static partial class PhySystem
    {
        #region 重叠查询
        /// <summary>
        /// 盒形重叠查询（外包盒，精确判定在窄相）
        /// </summary>
        /// <param name="size">盒全尺寸（各轴直径）</param>
        /// <param name="results">结果容器，调用前会被清空</param>
        /// <returns>命中数量</returns>
        public static int OverlapBox(Vector2 center, Vector2 size, float rotation, ulong layer,
                                     List<Handle> results)
        => CollectOverlap(OverlapBox(center, size, rotation, layer), results);

        /// <summary>
        /// 球形重叠查询
        /// </summary>
        /// <param name="rotation">仅为接口对称保留，球形本身无朝向</param>
        public static int OverlapSphere(Vector2 center, float radius, float rotation, ulong layer,
                                        List<Handle> results)
        => CollectOverlap(OverlapSphere(center, radius, rotation, layer), results);

        /// <summary>
        /// 胶囊重叠查询
        /// </summary>
        /// <param name="height">圆柱段高度（不含两端半球）</param>
        /// <param name="rotation">胶囊朝向角（弧度），胶囊轴为局部 Y</param>
        public static int OverlapCapsule(Vector2 center, float radius, float height, float rotation, ulong layer,
                                         List<Handle> results)
        => CollectOverlap(OverlapCapsule(center, radius, height, rotation, layer), results);

        private static int CollectOverlap(NativeArray raw, List<Handle> results)
        {
            results?.Clear();
            if(!raw)
                return 0;
            try
            {
                NativeArray<Handle> handles = raw.ToNativeArray<Handle>();
                int count = handles.array.size;
                if(results == null)
                    return count;
                for(int i = 0; i < count; i++)
                    results.Add(handles[i]);
                return count;
            }
            finally
            {
                raw.Dispose();
            }
        }
        #endregion

        #region 射线查询
        /// <summary>
        /// 单条射线检测，返回最近命中
        /// </summary>
        /// <param name="direction">射线方向；内部归一化，零向量视为未命中</param>
        /// <param name="hit">未命中时为 default（distance == 0），命中时 distance 为真实距离</param>
        /// <returns>是否命中</returns>
        public static bool Raycast(Vector2 origin, Vector2 direction, out RaycastHitInfo hit,
                                   float maxDis = float.MaxValue, ulong layer = DEFAULT_LAYER)
        {
            RaycastHitInfo result = default;
            bool hitAny;
            unsafe
            {
                hitAny = Raycast(origin, direction, &result, maxDis, layer);
            }
            hit = hitAny ? result : default;
            return hitAny;
        }

        /// <summary>
        /// 单条射线检测，返回全部命中（已按距离升序）
        /// </summary>
        /// <param name="results">结果容器，调用前会被清空</param>
        /// <returns>命中数量</returns>
        public static int RaycastAll(Vector2 origin, Vector2 direction, List<RaycastHitInfo> results,
                                     float maxDis = float.MaxValue, ulong layer = DEFAULT_LAYER)
        {
            results?.Clear();
            NativeArray raw = RaycastAll(origin, direction, maxDis, layer);
            if(!raw)
                return 0;
            try
            {
                NativeArray<RaycastHitInfo> hits = raw.ToNativeArray<RaycastHitInfo>();
                int count = hits.array.size;
                if(results == null)
                    return count;
                for(int i = 0; i < count; i++)
                    results.Add(hits[i]);
                return count;
            }
            finally
            {
                raw.Dispose();
            }
        }
        #endregion
    }
}
