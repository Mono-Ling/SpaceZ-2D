using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using SpaceZ_2D.Framework.Plugin;
using SpaceZ_2D.Framework.RunTime;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public static class Add
    {
#if UNITY_EDITOR
        private const string PLUGIN_NAME = "SpaceZ_2D_Alpha";
        private delegate int AddDelegate(int a, int b);
        private static NativeLoader _nativeLoader;
        private static AddDelegate _addFunction;

        [RuntimeStart(-100)]
        private static void Init()
        {
            try
            {
                _nativeLoader = new(PLUGIN_NAME);
                _nativeLoader.GetFunction("Add", out _addFunction);
            }
            catch (System.Exception ex)
            {
                Debug.LogError($"【Add】获取函数失败：{ex}");
            }
        }
        [RuntimeEnd(100)]
        private static void OnApplicationQuit()
        {
            _nativeLoader?.Dispose();
            _addFunction = null;
            _nativeLoader = null;
        }
        public static int NativeAdd(int a, int b)
        {
            if (_addFunction == null)
            {
                Debug.LogError("【Add】函数未初始化，请确保插件已正确加载。");
                return 0;
            }
            return _addFunction(a, b);
        }
#else
        [DllImport("SpaceZ_2D_Alpha", EntryPoint = "Add")]
        public static extern int NativeAdd(int a, int b);
#endif
    }
}