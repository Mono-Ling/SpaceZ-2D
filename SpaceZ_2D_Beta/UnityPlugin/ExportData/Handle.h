#pragma once
#include "Core/Object/Handle.h"

namespace UnityPlugin
{
    /// @brief 跨 DLL 边界的对象句柄
    struct Handle
    {
        int id = -1;
        unsigned int generation = 0;

        template<typename T>
        inline static Handle From(const Core::Handle<T>& handle)
        {
            Handle ans;
            ans.id = handle.Id();
            ans.generation = handle.Generation();
            return ans;
        }
        inline bool IsNull() const { return id < 0; }
    };
}
