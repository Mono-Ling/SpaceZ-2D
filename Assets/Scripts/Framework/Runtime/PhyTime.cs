using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Framework.RunTime
{
    public static class PhyTime
    {
        public const float DELAY_FRAME = 0.02f;
        public static float deltaTime { get; private set; }
        private static float _preFrame;
        public static void OnFrame()
        {
            deltaTime = _preFrame >= Mathf.Epsilon ? Time.realtimeSinceStartup - _preFrame : DELAY_FRAME;
            _preFrame = Time.realtimeSinceStartup;
        }
    }
}