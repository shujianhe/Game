using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.Script.Serialization;
using System.Web.Script.Services;
using System;
using System.Collections.Generic;
namespace SJHanHaiGameDataWeb
{
    public class SJUIPage
    {
        public static void OnPageLoad(System.Web.UI.Page page,ref Dictionary<string,object> dic,Func<bool> back)
        {
            int code = -1;
            string errorStr = "";
            string result = "";
            try
            {
                dic["code"] = 0;
                dic["errorStr"] = "";
                dic["debug"] = "";
                back();
            }
            catch (SJException ex)
            {
                dic["code"] = ex.code;
                dic["errorStr"] = ex.Str;
                dic["debug"] = ex.ToString();
            }
            finally
            {
                JavaScriptSerializer json = new JavaScriptSerializer();
                result = json.Serialize(dic);
                page.Response.Write(result);
            }
        }
    }
}