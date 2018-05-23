using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.Script.Serialization;
namespace SJHanHaiGameDataWeb
{
    public partial class test : System.Web.UI.Page
    {
        private void OnSelect(Dictionary<string, Dictionary<string, string>> param, ref Dictionary<string, object> DicRec)
        {            
            Dictionary<string, Dictionary<string, string>> result;
            List<string> list = new List<string>();
            foreach (KeyValuePair<string, string> kv in param["2"])
            {
                list.Add(kv.Value);
            }
            if (false == Tools.DBOper.select(param["1"]["cmdstr"], ref list, out result))
            {
                DicRec["errorcode"] = -3;
            }
            else
            {
                DicRec["data"] = result;// json.Serialize(result); 
            }
        }
        private void OnInsert(Dictionary<string, Dictionary<string, string>> param, ref Dictionary<string, object> DicRec)
        {
            Dictionary<int, Dictionary<string, object>> result;
            List<string> list = new List<string>();
            int i = 0;
            int sqlresult = Tools.DBOper.insert(param["1"]["cmdstr"], param["2"]);
            if (sqlresult < 1)
            {
                DicRec["errorcode"] = -3;
                DicRec["sqlerror"] = sqlresult;
            }
            else
            {
                DicRec["data"] = sqlresult;
            }
        }
        private void OnUpdate(Dictionary<string, Dictionary<string, string>> param, ref Dictionary<string, object> DicRec)
        {
            Dictionary<int, Dictionary<string, object>> result;
            List<string> list = new List<string>();
            int sqlresult = Tools.DBOper.update(param["1"]["cmdstr"], param["2"], param["3"]);
            if (sqlresult < 1)
            {
                DicRec["errorcode"] = -3;
                DicRec["sqlerror"] = sqlresult;
            }
            else
            {
                DicRec["data"] = sqlresult;
            }
        }
        protected void Page_Load(object sender, EventArgs e)
        {
            string senddata = Request["data"];
            string result = "";
            if (senddata == null || senddata == ""){
                result = "参数为空不能发送";
            }else{
                //result = "发送完成:结果 " + Tools.SJSocket.sendTo(senddata);
                result = "源字符串:" + senddata;
                Tools.tools.Encryption(ref senddata);
                result = "加密后:" + senddata;
            }
            Response.Write(result);
            /*string Type = "";
            string valuestr = "";
            Dictionary<string, object> DicRec = new Dictionary<string, object>();
            JavaScriptSerializer json = new JavaScriptSerializer();
            json.RecursionLimit = 20000;
            DicRec["errorcode"] = 0;
            try
            {
                Type = Request["Type"];
                DicRec["paramType"] = Type;
                valuestr = Request["valuestr"];
                DicRec["paramvaluestr"] = valuestr;
                Dictionary<string, Dictionary<string, string>> param = json.Deserialize<Dictionary<string, Dictionary<string, string>>>(valuestr);
                switch (Type)
                {
                    case "select":
                        {
                            OnSelect(param, ref DicRec);
                        } break;
                    case "insert":
                        {
                            OnInsert(param, ref DicRec);
                        } break;
                    case "update":
                        {
                            OnUpdate(param, ref DicRec);
                        } break;
                    default:
                        {
                            DicRec["errorcode"] = -2;
                        }
                        break;
                }
                DicRec["paramvaluestr"] = "";
            }
            catch (Exception ex)
            {
                DicRec["errorcode"] = -1;
                DicRec["ex"] = ex.ToString();
            }
            finally
            {
                try
                {
                    string strresult = json.Serialize(DicRec);
                    Tools.LoginFile.Write(strresult);
                    Response.Write(strresult);
                }
                catch (InvalidOperationException IOex)
                {
                    Response.Write("最后字典转字符串错误InvalidOperationException"+IOex.ToString());
                }
                catch (ArgumentException IOex)
                {
                    Response.Write("最后字典转字符串错误ArgumentException"+IOex.ToString());
                }
                catch (Exception IOex)
                {
                    Response.Write("最后字典转字符串错误其他错误:"+IOex.ToString());
                }
            }*/
        }
    }
}