using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 原生批量接口的写回调签名（首地址 + 元素个数），用于把托管缓冲一次性交给原生
    /// </summary>
    public unsafe delegate void NativeWriteFunc<T>(T* ptr, int count) where T : unmanaged;

    /// <summary>
    /// 可增长的连续缓冲，元素放在原生堆上（分配/释放走 <see cref="NativeArray.Create"/> / <see cref="NativeArray.Dispose"/>）
    /// <para>用途：托管侧攒好一批元素，再把 <see cref="GetPtr"/> + <see cref="Count"/> 直接喂给批量接口
    /// （SetBodiesProperty / SetBodiesVelocity / AddForces / SetBoxColliders 等）。</para>
    /// <para>注意：与 Unity.Collections.NativeList 同名但不依赖 Unity.Collections 包；
    /// 在别的命名空间同时 using 两者时需写全 SpaceZ_2D.Beta.NativeList。</para>
    /// <para>本类型是托管侧的便利容器，不属于原生映射层：它只保证内存连续与生命周期，不含任何业务语义。</para>
    /// </summary>
    public sealed class NativeList<T> : IDisposable where T : unmanaged
    {
        private const int MinCapacity = 8;

        private NativeArray<T> _buffer;
        private int _count;

        /// <param name="capacity">初始容量（元素个数），超出会自动翻倍增长</param>
        public NativeList(int capacity = MinCapacity)
        {
            if (capacity < 0)
                capacity = 0;
            _buffer = new NativeArray<T>(capacity);
        }

        /// <summary>已写入的元素个数</summary>
        public int Count => _count;

        /// <summary>当前分配的元素容量</summary>
        public int Capacity => _buffer.array.size;

        /// <summary>是否已持有原生内存</summary>
        public bool IsCreated => _buffer.array;

        public ref T this[int index]
        {
            get
            {
                if (index < 0 || index >= _count)
                    throw new IndexOutOfRangeException($"【NativeList】下标 {index} 越界，Count = {_count}");
                return ref _buffer[index];
            }
        }

        public unsafe T* GetPtr()
        => _count > 0 && _buffer.array.ptr != IntPtr.Zero ? (T*)_buffer.array.ptr.ToPointer() : null;

        public void Add(in T item)
        {
            EnsureCapacity(_count + 1);
            _buffer[_count++] = item;
        }

        public void AddRange(ReadOnlySpan<T> items)
        {
            if (items.Length == 0)
                return;
            EnsureCapacity(_count + items.Length);
            items.CopyTo(_buffer.AsSpan().Slice(_count, items.Length));
            _count += items.Length;
        }

        /// <summary>清空元素，保留已分配的容量</summary>
        public void Clear() => _count = 0;

        /// <summary>
        /// 把已写入的元素一次性交给原生批量接口，然后清空（容量保留）
        /// <para>空列表直接返回；回调为 null 时也清空，语义与"已消费"一致。</para>
        /// </summary>
        public unsafe void Sync(NativeWriteFunc<T> func)
        {
            if(_count <= 0)
                return;
            func?.Invoke(GetPtr(), _count);
            _count = 0;
        }

        /// <summary>已写入部分的视图（长度 = Count，不是 Capacity）</summary>
        public Span<T> AsSpan()
        => _count > 0 ? _buffer.AsSpan().Slice(0, _count) : Span<T>.Empty;

        /// <summary>只读视图，供只想遍历的场合使用</summary>
        public ReadOnlySpan<T> AsReadOnlySpan() => AsSpan();

        public void Dispose()
        {
            _buffer.Dispose();
            _count = 0;
        }

        /// <summary>
        /// 把容量扩到至少 min；按 2 倍增长，旧内容整体拷到新块后立刻释放旧块
        /// </summary>
        private void EnsureCapacity(int min)
        {
            int capacity = Capacity;
            if (capacity >= min)
                return;

            int next = capacity <= 0 ? MinCapacity : capacity;
            while (next < min)
                next *= 2;

            NativeArray<T> grown = new(next);
            if (_count > 0)
                _buffer.AsSpan().Slice(0, _count).CopyTo(grown.AsSpan());
            _buffer.Dispose();
            _buffer = grown;
        }
    }
}
