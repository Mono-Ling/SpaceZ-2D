using System;
using System.Collections;
using Unity.Collections;
using Unity.Collections.LowLevel.Unsafe;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public unsafe delegate void UnsafeFunc<T>(T* ptr, int count) where T : unmanaged;
    public unsafe delegate void UnsafeRefFunc<T>(T* ptr, ref int count) where T : unmanaged;
    public class NativeBuffer<T> : IDisposable where T : unmanaged
    {
        protected NativeList<T> _buffer;

        public void Init() => _buffer = new(Allocator.Persistent);
        public void Dispose()
        {
            if(_buffer.IsCreated)
                _buffer.Dispose();
        }
    }
    public class ReadOnlyNativeBuffer<T> : NativeBuffer<T> where T : unmanaged
    {
        public void Add(T value) => _buffer.Add(value);
        public void SyncAndClear(UnsafeFunc<T> func)
        {
            if(!_buffer.IsCreated || _buffer.IsEmpty)
                return;
            unsafe
            {
                T* ptr = (T*)_buffer.GetUnsafeReadOnlyPtr();
                func?.Invoke(ptr, _buffer.Length);
            }
            _buffer.Clear();
        }
    }
    public class NativeDataBuffer<T> : NativeBuffer<T> where T : unmanaged
    {
        public void GetNativeData(ref int count, UnsafeRefFunc<T> func)
        {
            if(!_buffer.IsCreated)
                return;
            _buffer.Clear();
            count = Mathf.Max(0, count);
            _buffer.Length = count;
            if(count == 0)
                return;
            unsafe
            {
                T* ptr = (T*)_buffer.GetUnsafePtr();
                func?.Invoke(ptr, ref count);
            }
            _buffer.Length = Mathf.Max(0, count);
        }
        public IEnumerator NativeDataList => _buffer.GetEnumerator();
    }
}