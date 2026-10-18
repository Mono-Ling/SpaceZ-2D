#pragma once
#include "Object/Handle.h"

namespace Export
{
    template<typename T>
    struct ExportHandle
    {
        int id = -1;
        int generation = 0;

        inline static ExportHandle<T> From(const Core::Handle<T>& handle)
        {
            ExportHandle<T> ans;
            ans.id = handle.Id();
            ans.generation = handle.Generation();
            return ans;
        }
    };
}