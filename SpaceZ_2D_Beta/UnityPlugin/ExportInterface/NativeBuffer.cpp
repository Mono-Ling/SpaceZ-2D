#include "UnityPlugin/ExportInterface/ExportInterface.h"
#include "UnityPlugin/ExportData/NativeArray.h"

namespace UnityPlugin
{
    EXPORT_FUNCTION NativeArray CreateBuffer(int size)
    {
        if(size <= 0)
            return {};
        TRY_CATCH_DEBUG
        (
            {
                return CreateNativeArray<unsigned char>(size);
            },
            "原生缓冲区",
            "原生数组创建失败"
        );
        return {};
    }
    EXPORT_FUNCTION void DestroyBuffer(NativeArray& array)
    {
        if(!array)
            return;
        TRY_CATCH_DEBUG
        (
            {
                DestroyNativeArray<unsigned char>(array);
            },
            "原生缓冲区",
            "原生数组销毁失败"
        );
    }
}