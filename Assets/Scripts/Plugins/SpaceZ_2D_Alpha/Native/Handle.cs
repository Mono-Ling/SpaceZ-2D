using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SpaceZ_2D.Alpha
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Handle : IEquatable<Handle>
    {
        public int id;
        public int generation;
        public override bool Equals(object obj)
        {
            if(obj is not Handle handle)
                return false;
            return Equals(handle);
        }
        public bool Equals(Handle other)
        => id == other.id && generation == other.generation;
        public override int GetHashCode()
        => (id, generation).GetHashCode();
        public override string ToString()
        => $"({id},{generation})";

        public static Handle NULL => _NULL;
        private static Handle _NULL = new(){id = -1, generation = 0};

        public static bool operator==(Handle a, Handle b)
        => a.Equals(b);
        public static bool operator!=(Handle a, Handle b)
        => !a.Equals(b);
    }
}