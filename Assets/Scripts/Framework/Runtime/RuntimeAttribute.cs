using System;
using System.Collections;
using System.Collections.Generic;

namespace SpaceZ_2D.Framework.RunTime
{
    public abstract class RuntimeAttribute : Attribute
    {
        public int Order { get; private set; }
        public RuntimeAttribute(int order = 0)
        {
            Order = order;
        }
    }
    [AttributeUsage(AttributeTargets.Method, Inherited = false, AllowMultiple = false)]
    public class RuntimeStartAttribute : RuntimeAttribute
    {
        public RuntimeStartAttribute(int order = 0) : base(order) { }
    }
    [AttributeUsage(AttributeTargets.Method, Inherited = false, AllowMultiple = false)]
    public class RuntimeEndAttribute : RuntimeAttribute
    {
        public RuntimeEndAttribute(int order = 0) : base(order) { }
    }
}
