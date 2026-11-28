using System;
using System.Runtime.InteropServices;

namespace SpaceZ_2D.Beta
{
    /// <summary>
    /// 跨 DLL 边界的对象句柄
    /// <para>对应原生 UnityPlugin::Handle（布局 = Core::Handle&lt;T&gt;）：id@0，generation@4，共 8 字节。</para>
    /// <para>刚体、碰撞体共用同一布局，原生侧靠模板标签区分种类，C# 侧不区分。</para>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Handle
    {
        public int id;
        public uint generation;

        /// <summary>空句柄（原生侧失败时返回 id == -1）</summary>
        public static Handle NULL => _NULL;
        private static readonly Handle _NULL = new() { id = -1, generation = 0 };

        public static bool operator ==(Handle a, Handle b)
        => a.id == b.id && a.generation == b.generation;

        public static bool operator !=(Handle a, Handle b) => !(a == b);

        public static implicit operator bool(Handle handle) => handle != NULL;

        public override bool Equals(object obj) => obj is Handle handle && this == handle;
        public override int GetHashCode() => HashCode.Combine(id, generation);
        public override string ToString() => $"({id}, {generation})";
    }
}
