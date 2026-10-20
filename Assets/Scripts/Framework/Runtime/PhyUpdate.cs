using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Framework.RunTime
{
    public class PhyLoop
    {
        public event Action update;
        private float _preFrame;
        public bool TryFrame()
        {
            if(Time.realtimeSinceStartup - _preFrame < PhyTime.DELAY_FRAME)
                return false;
            _preFrame = Time.realtimeSinceStartup;
            return true;
        }
        public void OnUpdate()
        {
            try
            {
                update?.Invoke();
            }
            catch (Exception e)
            {
                Debug.LogError($"【PhyLoop】帧更新异常{e}");
            }
        }
    }
    public class PhyUpdate
    {
        private static PhyLoop loop = new();
        public static void Add(Action action) => loop.update += action;
        public static void Remove(Action action) => loop.update -= action;
        public static void OnUpdate()
        {
            if(!loop.TryFrame())
                return;
            PhyTime.OnFrame();
            loop.OnUpdate();
        }
    }
    public class PhyLateUpdate
    {
        private static PhyLoop loop = new();
        public static void Add(Action action) => loop.update += action;
        public static void Remove(Action action) => loop.update -= action;
        public static void OnUpdate()
        {
            if(loop.TryFrame())
                loop.OnUpdate();
        }
    }
}