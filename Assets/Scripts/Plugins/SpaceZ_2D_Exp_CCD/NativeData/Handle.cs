using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SpaceZ_2D.Exp.CCD
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Handle
    {
        public int id;
        public int generation;
        override public bool Equals(object obj)
        {
            if (obj is Handle handle)
                return this == handle;
            return false;
        }
        override public int GetHashCode()
        => (id, generation).GetHashCode();
        public override string ToString()
        => $"({id}, {generation})";

        public static Handle NULL => _NULL;
        private static Handle   _NULL = new(){ id = -1, generation = 0};

        public static bool operator ==(Handle a, Handle b)
        => a.id == b.id && a.generation == b.generation;

        public static bool operator !=(Handle a, Handle b) => !(a == b);

        public static implicit operator bool(Handle handle)
        => handle != NULL;
    }
}