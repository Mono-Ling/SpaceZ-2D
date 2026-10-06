using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Reflection;


#if UNITY_EDITOR
using UnityEditor;
#endif

namespace SpaceZ_2D.Framework.RunTime
{
public static class RuntimeManager
{
    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.BeforeSceneLoad)]
    private static void RuntimeStart()
    {
        InvokeRuntimeAttributeMethods<RuntimeStartAttribute>();
#if UNITY_EDITOR
        EditorApplication.playModeStateChanged += OnPlayModeStateChanged;
#else
        Application.quitting += RuntimeEnd;
#endif
    }
    private static void RuntimeEnd()
    {
        InvokeRuntimeAttributeMethods<RuntimeEndAttribute>();
#if UNITY_EDITOR
        EditorApplication.playModeStateChanged -= OnPlayModeStateChanged;
#else
        Application.quitting -= RuntimeEnd;
#endif
    }
#if UNITY_EDITOR
    private static void OnPlayModeStateChanged(PlayModeStateChange state)
    {
        if (state == PlayModeStateChange.ExitingPlayMode)
        {
            RuntimeEnd();
            EditorApplication.playModeStateChanged -= OnPlayModeStateChanged;
        }
    }
#endif
    private static void InvokeRuntimeAttributeMethods<T>() where T : RuntimeAttribute
    {
        List<(int order, MethodInfo method)> methods = new();
        foreach (var asm in AppDomain.CurrentDomain.GetAssemblies())
        {
            Type[] types = null;
            try
            {
                types = asm.GetTypes();
            }
            catch (Exception ex)
            {
                Debug.LogError($"【RuntimeManager】获取程序集类型失败：{asm.FullName}，异常：{ex}");
                continue;
            }
            
            foreach (var type in types)
            {
                foreach (var method in type.GetMethods(
                    BindingFlags.Static
                  | BindingFlags.Public
                  | BindingFlags.NonPublic))
                {
                    var attr = method.GetCustomAttributes(typeof(T), false);
                    if (attr.Length > 0 && attr[0] is T runTimeAttr)
                        methods.Add((runTimeAttr.Order, method));
                }
            }
        }
        methods.Sort((a, b) => a.order.CompareTo(b.order));
        foreach (var (order, method) in methods)
        {
            try
            {
                method.Invoke(null, null);
            }
            catch (Exception ex)
            {
                Debug.LogError($"【RuntimeManager】{typeof(T).Name}方法执行失败：{ex}");
            }
        }
    }
}
}
