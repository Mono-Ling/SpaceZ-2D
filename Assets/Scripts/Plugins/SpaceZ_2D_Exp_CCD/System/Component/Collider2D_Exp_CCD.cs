using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    public static partial class PhySystem
    {
        public abstract class Collider2D_Exp_CCD : MonoBehaviour, INativeComponent
        {
            public Handle Handle => _handle;
            protected Handle _handle;
            public abstract void ComponentUpdate();
        }
    }
}