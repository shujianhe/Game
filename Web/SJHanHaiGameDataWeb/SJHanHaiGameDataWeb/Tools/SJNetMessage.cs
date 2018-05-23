using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace SJHanHaiGameDataWeb.Tools
{
    public class SJNetMessage
    {
        private byte[] data;
        private int offset;
        private int len;
        public int Offset
        {
            get
            {
                return offset;
            }
        }
        public byte[] Data
        {
            get
            {
                return data;
            }
        }
        private bool ByteArrAddTo(byte[] oldbuffer)
        {
            if (data.Length < offset + oldbuffer.Length)
            {
                //新buffer长度不足导致不能复制
                return false;
            }
            for (int i = 0; i < oldbuffer.Length; i++)
            {
                data[offset + i] = oldbuffer[i];
            }
            offset = offset + oldbuffer.Length;
            return true;
        }
        private bool ToByteArr(int value)
        {
            byte[] oldbuffer = BitConverter.GetBytes(value);
            return ByteArrAddTo(oldbuffer);
        }
        private bool DoubleToByteArr(double value)
        {
            byte[] oldbuffer = BitConverter.GetBytes(value);
            return ByteArrAddTo(oldbuffer);
        }
        private bool ToByteArr(bool value)
        {
            byte[] oldbuffer = BitConverter.GetBytes(value);
            return ByteArrAddTo(oldbuffer);
        }
        private bool ToByteArr(byte[] value)
        {
            return ByteArrAddTo(value);
        }
        private bool ToByteArr(string value)
        {
            byte[] oldbuffer = System.Text.Encoding.Default.GetBytes(value);
            return ByteArrAddTo(oldbuffer);
        }
        public void ResetOffset()
        {
            offset = 0;
        }
        private bool Get(ref int value)
        {
            try
            {
                value = BitConverter.ToInt32(data, offset);
                offset = offset + sizeof(int);
                return true;
            }
            catch
            {
                return false;
            }
        }
        private bool Get(ref bool value)
        {
            try
            {
                value = BitConverter.ToBoolean(data, offset);
                offset = offset + sizeof(bool);
                return true;
            }
            catch
            {
                return false;
            }
        }
        private bool Get(ref string value,int len)
        {
            try
            {
                if ( len > data.Length - offset)
                    throw new Exception();
                byte[] bytevalue = new byte[len];
                for (int i = 0; i < len; i++)
                {
                    bytevalue[i] = data[i + offset];
                }
                value = BitConverter.ToString(bytevalue);
                offset = offset + len;
                return true;
            }
            catch
            {
                return false;
            }
        }
        private bool Get(ref string value)
        {
            return Get(ref value, data.Length - offset);
        }
        public Dictionary<string,object> GetMsg()
        {
            ResetOffset();
            int len = 0;
            int timespan = 0;
            int ptype = 0;
            string value = "";
            Dictionary<string, object> result = null;
            if (Get(ref len) && Get(ref timespan)&&Get(ref ptype) && Get(ref value))
            {
                result = new Dictionary<string, object>();
                result["len"] = len;
                result["timespan"] = timespan;
                result["value"] = value;
            }
            return result;
        }
        public bool GetMsg(ref string value)
        {
            Dictionary<string, object> dir = GetMsg();
            if (dir == null || dir.ContainsKey("value"))
                return false;
            value = dir["value"] as string;
            return true;
        }
        public SJNetMessage(byte[] buffer)
        {
            offset = 0;
            if (buffer.Length < sizeof(int) * 4)
                throw new Exception("初始化buffer长度太少");
            data = buffer;
        }
        public SJNetMessage(string strdata)
        {
            byte[] bytedata = System.Text.Encoding.Default.GetBytes(strdata);
            this.data = new byte[sizeof(int) * 4 + bytedata.Length];
            len = strdata.Length;
            ToByteArr(strdata.Length);
            ToByteArr((int)tools.getTimeStamp());
            ToByteArr(0);
            ToByteArr(bytedata);
        }
    }
}