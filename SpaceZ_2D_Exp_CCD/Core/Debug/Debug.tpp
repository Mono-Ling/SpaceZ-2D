#include "Debug/Debug.h"
#include "Debug/ToString.h"

namespace Core::Debug
{
    inline void Log(const std::string& message)
    {
        for(auto& func : debugLog)
            if(func)
                func(message);
    }
    inline void LogWarning(const std::string& message)
    {
        for(auto& func : debugLogWarning)
            if(func)
                func(message);
    }
    inline void LogError(const std::string& message)
    {
        for(auto& func : debugLogError)
            if(func)
                func(message);
    }

    template<ToStringAbility T>
    void Log(const T& message)
    {
        Log(ToString(message));
    }
    template<ToStringAbility T>
    void LogWarning(const T& message)
    {
        LogWarning(ToString(message));
    }
    template<ToStringAbility T>
    void LogError(const T& message)
    {
        LogError(ToString(message));
    }
}