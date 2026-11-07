using System;
using System.Collections;
using System.Collections.Generic;
using Unity.Collections;
using Unity.Collections.LowLevel.Unsafe;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    public unsafe delegate void NativeReadFunc<T>(T* ptr, int count) where T : unmanaged;
    public unsafe delegate void NativeWriteFunc<T>(T* ptr, ref int count) where T : unmanaged;
    public abstract class NativeBuffer<T> : IDisposable where T : unmanaged
    {
        protected NativeList<T> _buffer;
        public void Init() => _buffer = new(Allocator.Persistent);

        public void Dispose()
        {
            if(_buffer.IsCreated)
                _buffer.Dispose();
        }
    }
    /// <summary>
    /// 原生侧读取缓冲区
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class NativeReadBuffer<T> : NativeBuffer<T> where T : unmanaged
    {
        public void Add(T value) => _buffer.Add(value);
        public unsafe void SynAndClear(NativeReadFunc<T> func)
        {
            if(!_buffer.IsCreated || _buffer.IsEmpty)
                return;
            T* ptr = _buffer.GetUnsafeReadOnlyPtr();
            func?.Invoke(ptr, _buffer.Length);

            _buffer.Clear();
        }
    }
    /// <summary>
    /// 原生侧写入缓冲区
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public class NativeWriteBuffer<T> : NativeBuffer<T> where T : unmanaged
    {
        public unsafe void GetNativeData(ref int count, NativeWriteFunc<T> func)
        {
            if(!_buffer.IsCreated || _buffer.IsEmpty)
                return;
            count = Mathf.Max(0, count);
            _buffer.Length = count;
            if(count == 0)
                return;
            T* ptr = _buffer.GetUnsafePtr();
            func?.Invoke(ptr, ref count);
            _buffer.Length = Mathf.Max(0, count);
        }
        public NativeList<T> NativeData => _buffer;
    }
}