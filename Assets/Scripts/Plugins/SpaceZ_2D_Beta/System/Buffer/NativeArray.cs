using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 原生侧缓冲区（对应原生 UnityPlugin::NativeArray：ptr@0 size@8，共 16 字节）
    /// <para>size 的语义由接口决定：<see cref="Create"/> 按<b>字节</b>，步进结果系列按<b>元素个数</b>。</para>
    /// <para>内存由原生侧 new[] 分配，必须调 <see cref="Dispose"/> 回收（内部走 DestroyBuffer 导出），否则整块泄漏。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct NativeArray
    {
        public IntPtr ptr;
        public int size;

        public NativeArray(IntPtr ptr, int size)
        {
            this.ptr = ptr;
            this.size = size;
        }

        /// <summary>空缓冲（ptr == null，size == 0）</summary>
        public static NativeArray NULL => new();

        public static implicit operator bool(NativeArray array)
        => array.ptr != IntPtr.Zero && array.size >= 0;

        public NativeArray<T> ToNativeArray<T>() where T : unmanaged
        => new(this);

        public override string ToString() => $"(0x{ptr.ToInt64():X}, {size})";

        #region 原生缓冲区 API（CreateBuffer / DestroyBuffer 的映射）
#if UNITY_EDITOR
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate NativeArray CreateBufferFunc(int size);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void DestroyBufferFunc(ref NativeArray array);

        [SpaceZ_2D.Framework.Plugin.NativeFunc("CreateBuffer")] private static CreateBufferFunc _createBuffer;
        [SpaceZ_2D.Framework.Plugin.NativeFunc("DestroyBuffer")] private static DestroyBufferFunc _destroyBuffer;

        private static NativeArray CreateBuffer(int size)
        => _createBuffer?.Invoke(size) ?? NULL;

        private static void DestroyBuffer(ref NativeArray array)
        => _destroyBuffer?.Invoke(ref array);
#else
        [DllImport(PhySystem.PLUGIN_NAME, EntryPoint = "CreateBuffer")]
        private static extern NativeArray CreateBuffer(int size);
        [DllImport(PhySystem.PLUGIN_NAME, EntryPoint = "DestroyBuffer")]
        private static extern void DestroyBuffer(ref NativeArray array);
#endif

        /// <summary>
        /// 分配一块原生缓冲
        /// </summary>
        /// <param name="bytes">字节数；非正数返回 <see cref="NULL"/></param>
        /// <returns>ptr 由原生侧 new[] 分配，size 为实际字节数</returns>
        /// <exception cref="Exception">原生分配失败（返回长度与请求不符）</exception>
        public static NativeArray Create(int bytes)
        {
            if (bytes <= 0)
                return NULL;
            NativeArray array = CreateBuffer(bytes);
            if (array.size != bytes || array.ptr == IntPtr.Zero)
                throw new Exception($"【NativeArray】原生分配失败：请求 {bytes} 字节，实际 {array.size}");
            return array;
        }

        /// <summary>
        /// 释放原生缓冲；重复调用安全
        /// </summary>
        public void Dispose()
        {
            if (ptr == IntPtr.Zero)
                return;
            DestroyBuffer(ref this);
        }
        #endregion
    }

    /// <summary>
    /// 强类型视图：只做指针 + 长度的包装，不持有内存所有权
    /// <para>array.size 是元素个数（不是字节数），由调用方保证与底层分配一致。</para>
    /// </summary>
    public struct NativeArray<T> : IEnumerable<T>, IDisposable where T : unmanaged
    {
        public NativeArray array;

        public NativeArray(NativeArray array)
        => this.array = array;

        /// <summary>
        /// 新建一块原生缓冲（size 是元素个数）
        /// </summary>
        public NativeArray(int size)
        {
            if (size <= 0)
            {
                array = NativeArray.NULL;
                return;
            }
            unsafe
            {
                array = NativeArray.Create(sizeof(T) * size);
            }
            array.size = size;
        }

        public void Dispose()
        => array.Dispose();

        public Span<T> AsSpan()
        {
            if(!array)
                return Span<T>.Empty;
            unsafe
            {
                return new((void*)array.ptr, array.size);
            }
        }
        public ref T this[int index]
        {
            get
            {
                if (index < 0 || index >= array.size)
                    throw new IndexOutOfRangeException();
                unsafe
                {
                    return ref ((T*)array.ptr)[index];
                }
            }
        }
        public IEnumerator<T> GetEnumerator()
        {
            if (!array)
                yield break;
            for (int i = 0; i < array.size; i++)
                yield return this[i];
        }

        IEnumerator IEnumerable.GetEnumerator() => GetEnumerator();
    }
}
