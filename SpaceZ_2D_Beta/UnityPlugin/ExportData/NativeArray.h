#pragma once
#include <cstddef>
#include <stdexcept>

namespace UnityPlugin
{
    struct NativeArray;

    template<typename T>
    NativeArray CreateNativeArray(std::size_t len);
    template<typename T>
    void DestroyNativeArray(NativeArray& array);
    
    /// @brief 封送缓冲数组
    /// @brief 不支持元素析构调用
    struct NativeArray
    {
    private:
        void* _ptr = nullptr;
        std::size_t _size = 0;

    public:
        explicit operator bool() const { return _ptr && _size; }
        template<typename T>
        inline T& At(std::size_t index)
        {
            if(!_ptr || index >= _size)
                throw std::out_of_range("NativeArray index out of range");
            T* typePtr = reinterpret_cast<T*>(_ptr);
            return typePtr[index];
        }
        template<typename T>
        inline const T& At(std::size_t index) const
        {
            if(!_ptr || index >= _size)
                throw std::out_of_range("NativeArray index out of range");
            const T* typePtr = reinterpret_cast<const T*>(_ptr);
            return typePtr[index];
        }

        template<typename T>
        friend NativeArray CreateNativeArray(std::size_t len);
        template<typename T>
        friend void DestroyNativeArray(NativeArray& array);

        static const NativeArray null;
    };
    inline const NativeArray NativeArray::null{};

    template<typename T>
    inline NativeArray CreateNativeArray(std::size_t len)
    {
        NativeArray arr;
        if(!len)
            return arr;
        arr._ptr = new T[len];
        arr._size = len;
        return arr;
    }
    template<typename T>
    inline void DestroyNativeArray(NativeArray& array)
    {
        if(!array)
            return;
        ::operator delete[](array._ptr);
        array._ptr = nullptr;
        array._size = 0;
    }
}