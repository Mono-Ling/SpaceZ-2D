#pragma once
#include "UnityPlugin/Profiler/Profiler.h"

namespace UnityPlugin
{
    struct ProfilerMarker
    {
    private:
        const UnityProfilerMarkerDesc* _marker = nullptr;

    public:
        ProfilerMarker() = default;
        ProfilerMarker(const char* name, bool relase = false)
        {
            if(!g_profiler)
            {
                OnProfilerLoad().emplace_back([this, name, relase]()
                {
                    if(!g_profiler)
                        return;
                    if(!relase && !g_profiler->IsAvailable())
                        return;
                    g_profiler->CreateMarker
                    (
                        &_marker,
                        name,
                        kUnityProfilerCategoryPhysics2D,
                        static_cast<UnityProfilerMarkerFlags>
                        (
                            relase ? kUnityProfilerMarkerFlagAvailabilityNonDev
                                   : kUnityProfilerMarkerFlagDefault
                        ),
                        0
                    );
                });
                return;
            }
            if(!relase && !g_profiler->IsAvailable())
                return;
            g_profiler->CreateMarker
            (
                &_marker,
                name,
                kUnityProfilerCategoryPhysics2D,
                static_cast<UnityProfilerMarkerFlags>
                (
                    relase ? kUnityProfilerMarkerFlagAvailabilityNonDev
                           : kUnityProfilerMarkerFlagDefault
                ),
                0
            );
        }

        inline void BeginSample() const
        {
            if(g_profiler && _marker)
                g_profiler->BeginSample(_marker);
        }

        inline void EndSample() const
        {
            if(g_profiler && _marker)
                g_profiler->EndSample(_marker);
        }

        friend ProfilerMarker CreateMarker(const char* flag);
        friend ProfilerMarker CreateReleaseMarker(const char* flag);
    };

    struct TempProfilerMarker
    {
    private:
        const ProfilerMarker& marker;

    public:
        explicit TempProfilerMarker(const ProfilerMarker& m)
        : marker(m) { marker.BeginSample(); }
        TempProfilerMarker(ProfilerMarker&& m) = delete;
        TempProfilerMarker(const TempProfilerMarker&) = delete;
        ~TempProfilerMarker() { marker.EndSample(); }

        TempProfilerMarker& operator=(const TempProfilerMarker&) = delete;
    };
}