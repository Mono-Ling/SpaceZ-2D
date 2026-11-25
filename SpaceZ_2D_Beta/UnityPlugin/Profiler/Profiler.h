#pragma once
#include "UnityPlugin/Profiler/IUnityProfiler.h"
#include "UnityPlugin/UnityPlugin/UnityPlugin.h"
#include <functional>

namespace UnityPlugin
{
    inline IUnityProfiler* g_profiler = nullptr;

    struct ProfilerMarker;
    ProfilerMarker CreateMarker(const char* name);
    ProfilerMarker CreateReleaseMarker(const char* name);

    using ProfilerCallbackFunc = std::function<void()>;
    inline Action<ProfilerCallbackFunc>& OnProfilerLoad()
    {
        static Action<ProfilerCallbackFunc> callback;
        return callback;
    }
}