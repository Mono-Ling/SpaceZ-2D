using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Framework.Plugin
{
    public static class PluginUtility
    {
        public static string Path{get;private set;}
        static PluginUtility()
        {
    #if UNITY_EDITOR_64
            Path = Application.dataPath + "/Plugins/x86_64";
    #else
            throw new System.Exception("PluginUtility only supports 64-bit Unity Editor.");
    #endif
        }
        public static string GetDllPath(string pluginName)
        => $"{Path}/{pluginName}.dll";
    }
}