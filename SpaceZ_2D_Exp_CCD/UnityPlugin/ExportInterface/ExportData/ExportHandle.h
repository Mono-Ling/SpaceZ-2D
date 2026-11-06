#pragma once
#include "Object/Handle.h"

namespace ExportData
{
    struct ExportHandle
    {
        int id = -1;
        int generation = 0;

        template<typename T>
        inline static ExportHandle From(const Core::Handle<T>& handle)
        {
            ExportHandle ans;
            ans.id = handle.Id();
            ans.generation = handle.Generation();
            return ans;
        }
    };
}