using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SpaceZ_2D.Framework.Plugin
{
    public class NativeLoader : IDisposable
    {
        private class NativeLibrary : IDisposable
        {
            public int UserCount { get; private set; }
            public IntPtr Handle { get; private set; }
            public string Path { get; private set; }

            public NativeLibrary(string path)
            {
                Handle = LoadLibrary(path);
                if (Handle == IntPtr.Zero)
                    throw new Exception($"【NativeLoader】Failed to load library: {path}");
                this.Path = path;
                UserCount = 1;
            }
            public static NativeLibrary Load(string path)
            {
                if (_loadedLibraries.TryGetValue(path, out var library))
                {
                    library.UserCount++;
                    return library;
                }
                NativeLibrary newLibrary = new(path);
                if (newLibrary.Handle != IntPtr.Zero)
                    _loadedLibraries.Add(path, newLibrary);
                return newLibrary;
            }
            public void Unload()
            {
                UserCount--;
                if (UserCount <= 0)
                    Dispose();
            }

            public void Dispose()
            {
                if (Handle == IntPtr.Zero)
                    return;
                FreeLibrary(Handle);
                Handle = IntPtr.Zero;
                _loadedLibraries.Remove(Path);
            }
        }

        #region Platform APIs
        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Unicode)]
        private static extern IntPtr LoadLibrary([MarshalAs(UnmanagedType.LPWStr)] string path);
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool FreeLibrary(IntPtr handle);
        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Ansi)]
        private static extern IntPtr GetProcAddress(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string procName);
        #endregion

        private static Dictionary<string, NativeLibrary> _loadedLibraries = new();

        private NativeLibrary _library;
        public NativeLoader(string path)
        {
            var library = NativeLibrary.Load(path);
            _library = library.Handle != IntPtr.Zero ? library : null;
        }
        public bool GetFunction<T>(string functionName, out T function) where T : Delegate
        {
            function = null;
            if (_library == null)
                return false;
            IntPtr procAddress = GetProcAddress(_library.Handle, functionName);
            if (procAddress == IntPtr.Zero)
                return false;
            function = Marshal.GetDelegateForFunctionPointer<T>(procAddress);
            return true;
        }
        public void Dispose()
        {
            if (_library == null)
                return;
            _library.Unload();
            _library = null;
        }
    }
}