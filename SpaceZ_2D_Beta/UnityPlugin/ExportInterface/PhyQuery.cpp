#include "UnityPlugin/ExportInterface/ExportInterface.h"
#include "UnityPlugin/Profiler/ProfilerMarker.h"
#include "UnityPlugin/ExportData/NativeArray.h"
#include "UnityPlugin/ExportData/Handle.h"
#include "UnityPlugin/ExportData/RaycastHit.h"
#include <vector>
using namespace Core;
using namespace Core::Math;

// 空间查询导出：重叠查询与射线检测。
//   命中结果用 CreateNativeArray 新建、按 NativeArray 返回，调用方用完必须调 DestroyBuffer 释放；
//   NativeArray 只释放裸内存、不析构元素，元素类型须平凡析构的（如 Handle、 RaycastHit ）。
//
// layer 参数一律传 64 位掩码而不是 LayerMask 结构体；传 0 表示不匹配任何层。

namespace UnityPlugin
{
    // Profiler 标记在静态存储期构造：此时 g_profiler 还是空，构造体只会把创建回调挂到
    // OnProfilerLoad()（内部用 NativeAllocator，不走被替换的全局 operator new），
    // 等 UnityPluginLoad 拿到 IUnityProfiler 后再真建标记 —— 与 PhyEngine.cpp 的 Step 标记同一条路径
    static ProfilerMarker g_overlapBoxMarker("【SpaceZ-2D】OverlapBox");
    static ProfilerMarker g_overlapSphereMarker("【SpaceZ-2D】OverlapSphere");
    static ProfilerMarker g_overlapCapsuleMarker("【SpaceZ-2D】OverlapCapsule");
    static ProfilerMarker g_raycastMarker("【SpaceZ-2D】Raycast");
    static ProfilerMarker g_raycastAllMarker("【SpaceZ-2D】RaycastAll");

#pragma region 重叠查询
    /// @brief 盒形重叠查询（外包盒，不是精确求交，精确判定在窄相）
    /// @param center 盒中心（世界坐标）
    /// @param size 盒**全尺寸**（各轴直径），内部会除 2 当半长用
    /// @param rotation 盒旋转角（弧度）
    /// @param layer 查询层掩码
    /// @return NativeArray<UnityPlugin::Handle>，元素是命中的碰撞体句柄；无命中返回空
    EXPORT_FUNCTION NativeArray OverlapBox(Vector2 center, Vector2 size, float rotation, unsigned long long layer)
    {
        if(!PhyAvailable())
            return {};
        TempProfilerMarker marker(g_overlapBoxMarker);
        TRY_CATCH_DEBUG
        (
            {
                auto handles = g_phySystem->OverlapBox(center, size, rotation, Collision::LayerMask(layer));
                int count = static_cast<int>(handles.size());
                if(count <= 0)
                    return {};

                NativeArray ans = CreateNativeArray<Handle>(count);
                for(int i = 0; i < count; i++)
                    At<Handle>(ans, i) = Handle::From(handles[i]);
                return ans;
            },
            "PhyQuery",
            "盒形重叠查询异常"
        );
        return {};
    }
    /// @brief 球形重叠查询
    /// @param radius 半径
    /// @param rotation 仅为接口对称保留，球形本身无朝向
    /// @return NativeArray<UnityPlugin::Handle>
    EXPORT_FUNCTION NativeArray OverlapSphere(Vector2 center, float radius, float rotation, unsigned long long layer)
    {
        if(!PhyAvailable())
            return {};
        TempProfilerMarker marker(g_overlapSphereMarker);
        TRY_CATCH_DEBUG
        (
            {
                auto handles = g_phySystem->OverlapSphere(center, radius, rotation, Collision::LayerMask(layer));
                int count = static_cast<int>(handles.size());
                if(count <= 0)
                    return {};

                NativeArray ans = CreateNativeArray<Handle>(count);
                for(int i = 0; i < count; i++)
                    At<Handle>(ans, i) = Handle::From(handles[i]);
                return ans;
            },
            "PhyQuery",
            "球形重叠查询异常"
        );
        return {};
    }
    /// @brief 胶囊重叠查询
    /// @param radius 半径
    /// @param height 圆柱段高度（不含两端半球）
    /// @param rotation 胶囊朝向角（弧度），胶囊轴为局部 Y
    /// @return NativeArray<UnityPlugin::Handle>
    EXPORT_FUNCTION NativeArray OverlapCapsule(Vector2 center, float radius, float height, float rotation, unsigned long long layer)
    {
        if(!PhyAvailable())
            return {};
        TempProfilerMarker marker(g_overlapCapsuleMarker);
        TRY_CATCH_DEBUG
        (
            {
                auto handles = g_phySystem->OverlapCapsule(center, radius, height, rotation, Collision::LayerMask(layer));
                int count = static_cast<int>(handles.size());
                if(count <= 0)
                    return {};

                NativeArray ans = CreateNativeArray<Handle>(count);
                for(int i = 0; i < count; i++)
                    At<Handle>(ans, i) = Handle::From(handles[i]);
                return ans;
            },
            "PhyQuery",
            "胶囊重叠查询异常"
        );
        return {};
    }
#pragma endregion

#pragma region 射线查询
    /// @brief 单条射线检测，返回最近命中
    /// @param origin 射线起点
    /// @param direction 射线方向；内部会归一化，长度不影响结果，传零向量视为未命中
    /// @param hit 命中结果出参（指针由调用方 pin 住）；**未命中时 distance 置 -1**
    /// @param maxDis 最大检测距离（世界单位）
    /// @param layer 查询层掩码
    /// @return 是否命中
    /// @note C# 侧返回值要按 1 字节封送（[return: MarshalAs(UnmanagedType.I1)]），默认 4 字节 BOOL 会读到高位垃圾
    EXPORT_FUNCTION bool Raycast(Vector2 origin, Vector2 direction, RaycastHit* hit, float maxDis, unsigned long long layer)
    {
        if(!PhyAvailable() || !hit)
            return false;
        TempProfilerMarker marker(g_raycastMarker);
        *hit = RaycastHit{};
        TRY_CATCH_DEBUG
        (
            {
                Core::RaycastHit coreHit;
                if(!g_phySystem->Raycast(origin, direction, coreHit, maxDis, Collision::LayerMask(layer)))
                    return false;
                *hit = RaycastHit(coreHit);
                return true;
            },
            "PhyQuery",
            "射线检测异常"
        );
        return false;
    }
    /// @brief 单条射线检测，返回全部命中
    /// @param direction 射线方向；内部会归一化
    /// @param maxDis 最大检测距离（世界单位）
    /// @return NativeArray<UnityPlugin::RaycastHit>，已按 distance 升序排好；无命中返回空
    EXPORT_FUNCTION NativeArray RaycastAll(Vector2 origin, Vector2 direction, float maxDis, unsigned long long layer)
    {
        if(!PhyAvailable())
            return {};
        TempProfilerMarker marker(g_raycastAllMarker);
        TRY_CATCH_DEBUG
        (
            {
                auto hits = g_phySystem->RaycastAll(origin, direction, maxDis, Collision::LayerMask(layer));
                int count = static_cast<int>(hits.size());
                if(count <= 0)
                    return {};

                NativeArray ans = CreateNativeArray<RaycastHit>(count);
                for(int i = 0; i < count; i++)
                    At<RaycastHit>(ans, i) = RaycastHit(hits[i]);
                return ans;
            },
            "PhyQuery",
            "射线检测异常"
        );
        return {};
    }
#pragma endregion
}
