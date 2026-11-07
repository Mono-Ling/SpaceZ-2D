using System.Collections;
using System.Collections.Generic;

namespace SpaceZ_2D.Exp.CCD
{
    public static partial class PhySystem
    {
        public interface INativeComponent
        {
            Handle Handle { get; }
            void ComponentUpdate();
        }
    }
}