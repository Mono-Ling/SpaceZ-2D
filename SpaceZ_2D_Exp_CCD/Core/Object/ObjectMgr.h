#pragma once
#include "Object/Handle.h"
#include <vector>
#include <stack>
#include <utility>
#include <memory>
#include <functional>

namespace Core::Object
{
    template<typename T>
    class ObjectMgr
    {
        std::stack<Handle<T>> _buffer;
        std::vector<std::pair<Handle<T>, std::unique_ptr<T>>> _objs;
    
    public:
        Handle<T> PeekNextHandle() const;
        bool Contains(const Handle<T>& handle) const;
        bool TryAdd(const Handle<T>& handle , std::unique_ptr<T>&& ptr);
        bool TryAdd(const std::function<std::unique_ptr<T>(const Handle<T>&)>& creator);
        bool TryRemove(const Handle<T>& handle);

        /// @brief 尝试获取对象（禁止外部释放指针）
        /// @param handle 句柄
        /// @param ptr 指针引用
        /// @return 是否获取成功
        bool TryGet(const Handle<T>& handle, T*& ptr);
        void ForEach(const std::function<void(const Handle<T>&, T&)>& func);

    private:
        void Carry();
        bool IsFree(const Handle<T>& handle) const;
    };
}
#include "Object/ObjectMgr.tpp"