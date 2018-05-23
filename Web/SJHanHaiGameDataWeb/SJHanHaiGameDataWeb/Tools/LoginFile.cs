using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.IO;
namespace SJHanHaiGameDataWeb.Tools
{
    public class LoginFile
    {
        private static string FileName;// = "";
        private static string path = HttpContext.Current.Request.PhysicalApplicationPath + "logs";
        private static string DateGetFileName(string head,DateTime dt){
            return "/"+head + dt.ToString("yyyy-MM-dd") + ".log"; 
        }
        private static string getDatePrve(string head, DateTime dt){
            return head + dt.ToString("yyyy-MM-dd HH:mm:ss.fff");//string.Format("[[%s: %d_%d_%d %d::%d::%d]]");
        }
        static LoginFile()
        {
            DateTime date = System.DateTime.Now;
            FileName = DateGetFileName("debug", date);
            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }
        }
        public static bool Write(string s)
        {
            DateTime dt = DateTime.Now;
            string tempfilename = DateGetFileName("debug", dt);
            if (tempfilename != FileName){
                FileName = tempfilename;
            }
            StreamWriter sw = File.AppendText(path + FileName);
            sw.Write(getDatePrve("debug", dt) + s +"\r\n");
            sw.Close();
            return true;
        }
    }
}