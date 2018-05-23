using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace SJHanHaiGameDataWeb.Tools
{
    public class InstanceData
    {
        private static Dictionary<string, object> root = new Dictionary<string, object>();
        public static void set(string k,object v)
        {
            root[k] = v;
        }
        public static object get(string k,Func<object> NilHandle)
        {
            if(root.ContainsKey(k) == false)
            {
                object v = NilHandle();
                if (v != null)
                {
                    root[k] = v;
                    return root[k];
                }
                return null;
            }
            return root[k];
        }
        public static object get(string k)
        {
            Func<object> temp = delegate(){
                return null;
            };
            return get(k,temp);
        }
    }
}