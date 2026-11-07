using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;

namespace SpaceZ_2D.Framework.Plugin
{
    public class AutoNativeLoader : NativeLoader
    {
        public AutoNativeLoader(string pluginName) : base(pluginName) {}

        public void Load(Type type)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));

            var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static);
            foreach (var field in fields)
            {
                var attr = Attribute.GetCustomAttribute(field, typeof(NativeFuncAttribute)) as NativeFuncAttribute;
                if(attr == null)
                    continue;
                if(!typeof(Delegate).IsAssignableFrom(field.FieldType))
                    throw new Exception($"【AutoNativeLoader】Field {field.Name} is not a delegate type.");
                
                var function = GetFunction(attr.funcName, field.FieldType);
                if(function == null)
                    throw new Exception($"【AutoNativeLoader】Failed to load function {attr.funcName} for field {field.Name}.");
                field.SetValue(null, function);
            }
        }
        public void UnLoad(Type type)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));

            var fields = type.GetFields(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static);
            foreach (var field in fields)
            {
                var attr = Attribute.GetCustomAttribute(field, typeof(NativeFuncAttribute)) as NativeFuncAttribute;
                if(attr == null)
                    continue;
                if(!typeof(Delegate).IsAssignableFrom(field.FieldType))
                    throw new Exception($"【AutoNativeLoader】Field {field.Name} is not a delegate type.");
                
                field.SetValue(null, null);
            }
        }
    }
}