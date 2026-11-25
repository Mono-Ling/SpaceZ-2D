#include "UnityPlugin/Profiler/Profiler.h"
#include "UnityPlugin/Profiler/ProfilerMarker.h"
#include "UnityPlugin/UnityPlugin/UnityPlugin.h"

namespace UnityPlugin
{
    static void LoadProfilerInterface(IUnityInterfaces* ptr)
    {
        if(!ptr)
            return;
        g_profiler = ptr->Get<IUnityProfiler>();
        if(!g_profiler)
            return;

        for(auto& func : OnProfilerLoad())
            if(func)
                func();
        OnProfilerLoad().clear();
    }
    static void UnloadProfilerInterface()
    {
        g_profiler = nullptr;
    }

    static bool init = []() -> bool
    {
        OnUnityLoadCallbacks().emplace_back(LoadProfilerInterface);
        OnUnityUnloadCallbacks().emplace_back(UnloadProfilerInterface);

        return true;
    }();

    ProfilerMarker CreateMarker(const char* flag)
    {
        ProfilerMarker marker;
        if(!g_profiler || !g_profiler->IsAvailable())
            return marker;
        g_profiler->CreateMarker(
            &marker._marker,
            flag,
            kUnityProfilerCategoryPhysics2D,
            kUnityProfilerMarkerFlagDefault,
            0);
        return marker;
    }
    ProfilerMarker CreateReleaseMarker(const char* flag)
    {
        ProfilerMarker marker;
        if(!g_profiler)
            return marker;
        g_profiler->CreateMarker(
            &marker._marker,
            flag,
            kUnityProfilerCategoryPhysics2D,
            kUnityProfilerMarkerFlagAvailabilityNonDev,
            0);
        return marker;
    }
}