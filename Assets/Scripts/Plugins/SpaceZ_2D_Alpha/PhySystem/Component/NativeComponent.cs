using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public static partial class PhySystem
    {
        public abstract class NativeComponent<T> : MonoBehaviour
        {
            public Handle Handle => _handle;
            protected Handle _handle = Handle.NULL;

            protected bool IsEnable(Dictionary<Handle, T> dic)
            {
                if(_handle == Handle.NULL || !(dic?.ContainsKey(_handle) ?? false))
                {
                    Debug.LogWarning("【原生组件】无效句柄");
                    return false;
                }
                return true;
            }
        }
    }
}