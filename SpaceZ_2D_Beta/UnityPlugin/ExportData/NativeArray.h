#pragma once
#include <cstddef>
#include <stdexcept>

namespace UnityPlugin
{
    /// @brief 封送缓冲数组
    /// @brief 不支持元素析构调用
    struct NativeArray
    {
        void* ptr = nullptr;
        int size = 0;

        explicit operator bool() const { return ptr && size > 0; }
    };

    template<typename T>
    inline NativeArray CreateNativeArray(int len)
    {
        NativeArray arr;
        if(len <= 0)
            return arr;
        arr.ptr = new T[len];
        arr.size = len;
        return arr;
    }
    template<typename T>
    inline void DestroyNativeArray(NativeArray& array)
    {
        if(!array)
            return;
        ::operator delete[](array.ptr);
        array.ptr = nullptr;
        array.size = 0;
    }

    template<typename T>
    inline T& At(NativeArray& array, int index)
    {
        if(!array.ptr || index < 0 || index >= array.size)
            throw std::out_of_range("NativeArray index out of range");
        T* typePtr = reinterpret_cast<T*>(array.ptr);
        return typePtr[index];
    }
    template<typename T>
    inline const T& At(const NativeArray& array, int index)
    {
        if(!array.ptr || index < 0 || index >= array.size)
            throw std::out_of_range("NativeArray index out of range");
        T* typePtr = reinterpret_cast<T*>(array.ptr);
        return typePtr[index];
    }
}