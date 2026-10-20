using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.LowLevel;
using UnityEngine.PlayerLoop;

namespace SpaceZ_2D.Framework.RunTime
{
    public static class UpdateManager
    {
        [RuntimeStart(-100)]
        private static void Init()
        {
            PlayerLoopSystem loop = PlayerLoop.GetCurrentPlayerLoop();
            PlayerLoopSystem insert = new()
            {
                type = typeof(PhyLoop),
                updateDelegate = null,
                subSystemList = new PlayerLoopSystem[]
                {
                    new()
                    {
                        type = typeof(PhyUpdate),
                        updateDelegate = PhyUpdate.OnUpdate,
                        subSystemList = null,
                    },
                    new()
                    {
                        type = typeof(PhyLateUpdate),
                        updateDelegate = PhyLateUpdate.OnUpdate,
                        subSystemList = null,
                    },
                }
            };
            if(!LoopUtility.InsertLoop(ref loop, insert, typeof(PreUpdate)))
            {
                Debug.LogError("【帧更新管理器】物理循环插入失败");
                return;
            }
            PlayerLoop.SetPlayerLoop(loop);
        }
        [RuntimeEnd(100)]
        private static void OnApplicationQuit()
        => PlayerLoop.SetPlayerLoop(PlayerLoop.GetDefaultPlayerLoop());
    }
}