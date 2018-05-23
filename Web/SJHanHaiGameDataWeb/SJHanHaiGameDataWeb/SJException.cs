using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace SJHanHaiGameDataWeb
{
    public class SJException : ApplicationException
    {
        private int errorcode;
        private string errorStr;
        private Exception innerException;
        public int code
        {
            get
            {
                return errorcode;
            }
            set
            {
                errorcode = value;
            }
        }
        public string Str
        {
            get
            {
                return errorStr;
            }
            set
            {
                errorStr = value;
            }
        }
        public SJException()
        {
            code = 0;
            Str = "";
        }
        public SJException(string msg):base(msg)
        {
            Str = msg;
            code = 0;
        }
        public SJException(string msg,Exception ex)
        {
            code = 0;
            Str = msg;
            innerException = ex;
        }
        int getcode()
        {
            return errorcode;
        }
        string getStr()
        {
            return errorStr;
        }
    }
}