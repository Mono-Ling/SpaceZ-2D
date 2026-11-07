using System;
using System.Collections;
using System.Collections.Generic;

namespace SpaceZ_2D.Framework.Plugin
{
    [AttributeUsage(AttributeTargets.Field, AllowMultiple = false, Inherited = false)]
    public class NativeFuncAttribute : Attribute
    {
        public string funcName;
        public NativeFuncAttribute(string funcName)
        => this.funcName = funcName;
    }
}
