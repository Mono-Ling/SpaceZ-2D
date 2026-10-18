#pragma once
#include "Object/Handle.h"

namespace Export
{
    template<typename Obj, typename Info>
    struct ObjectInfo
    {
        Core::Handle<Obj> handle;
        Info info;
    };
}