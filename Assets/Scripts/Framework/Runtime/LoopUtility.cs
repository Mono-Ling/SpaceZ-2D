using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.LowLevel;

namespace SpaceZ_2D.Framework.RunTime
{
    public static class LoopUtility
    {
        public static bool InsertLoop(ref PlayerLoopSystem curr, PlayerLoopSystem insert, Type after)
        {
            if(curr.subSystemList == null || curr.subSystemList.Length == 0)
                return false;
            for(int i = 0; i < curr.subSystemList.Length; i++)
            {
                if(curr.subSystemList[i].type == after)
                {
                    List<PlayerLoopSystem> temp = new(curr.subSystemList);
                    temp.Insert(i, insert);
                    curr.subSystemList = temp.ToArray();
                    return true;
                }
                if(InsertLoop(ref curr.subSystemList[i], insert, after))
                    return true;
            }
            return false;
        }
    }
}