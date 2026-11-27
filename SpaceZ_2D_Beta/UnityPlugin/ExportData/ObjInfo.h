#pragma once
#include "Core/Object/Handle.h"

namespace UnityPlugin
{
    /// @brief 批量接口的「句柄 + 载荷」对
    /// @tparam T 句柄标签类型（Rigidbody / Collider），只用于区分句柄种类，不参与布局
    /// @tparam W 载荷类型，C# 侧逐字段展平成同布局的 struct 后按指针传入
    /// @note 本结构只作为**入参指针**使用，绝不作为返回值，因此不受 C4190 约束；
    ///       布局 = 8 字节句柄（id @0，generation @4）+ W
    template<typename T, typename W>
    struct ObjInfo
    {
        Core::Handle<T> handle;
        W info;
    };
}
