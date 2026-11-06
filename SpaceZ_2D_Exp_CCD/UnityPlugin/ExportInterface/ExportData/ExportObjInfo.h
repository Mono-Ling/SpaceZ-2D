#pragma once
#include "Object/Handle.h"

namespace ExportData
{
    template<typename T, typename W>
    struct ExportObjInfo
    {
        Core::Handle<T> handle;
        W info;
    };
}