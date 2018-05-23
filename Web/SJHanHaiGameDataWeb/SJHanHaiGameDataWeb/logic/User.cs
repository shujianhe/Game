using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace SJHanHaiGameDataWeb.logic
{
    public class User
    {
        static List<string> SQLAccoutTableKeyList;
        static User(){
            SQLAccoutTableKeyList = new List<string>();
            SQLAccoutTableKeyList.Add("AccountId");
            SQLAccoutTableKeyList.Add("gameid");
            SQLAccoutTableKeyList.Add("Platformid");
            SQLAccoutTableKeyList.Add("loginAccount");
            SQLAccoutTableKeyList.Add("password");
            SQLAccoutTableKeyList.Add("nikeName");
            SQLAccoutTableKeyList.Add("imei");
            SQLAccoutTableKeyList.Add("imgid");
            SQLAccoutTableKeyList.Add("sex");
            SQLAccoutTableKeyList.Add("createtime");
            SQLAccoutTableKeyList.Add("lastlogintime");
            SQLAccoutTableKeyList.Add("createArea");
            SQLAccoutTableKeyList.Add("lastArea");
            SQLAccoutTableKeyList.Add("tel");
            SQLAccoutTableKeyList.Add("szToken");
        }
        private static Dictionary<string,object> getParam(HttpRequest Request, Dictionary<string, string> Name)
        {
            SJException SJE = new SJException();
            Dictionary<string, object> result = new Dictionary<string, object>();            
            foreach (var item in Name)
            {
                string str = Request[item.Key];
                if(str == null)
                {
                    SJE.code = -2;
                    SJE.Str = "缺少参数:" + item.Key;
                    throw SJE;
                }
                switch (item.Value)
                {
                    case "int":
                        result[item.Key] = Convert.ToInt32(str);
                        break;
                    case "int32":
                        result[item.Key] = Convert.ToInt32(str);
                        break;
                    case "int64":
                        result[item.Key] = Convert.ToInt64(str);
                        break;
                    case "int16":
                        result[item.Key] = Convert.ToInt16(str);
                        break;
                    case "bool":
                        result[item.Key] = Convert.ToBoolean(str);
                        break;
                    case "Boolean":
                        result[item.Key] = Convert.ToBoolean(str);
                        break;
                    case "double":
                        result[item.Key] = Convert.ToDouble(str);
                        break;
                    case "string":
                        result[item.Key] = str;
                        break;
                    default:
                        {
                            SJE.code = -3;
                            SJE.Str = "未知的强转类型:" + item.Value;
                            throw SJE;
                        }
                }
            }
            return result;
        }
        public static void Login(HttpRequest Request,ref Dictionary<string,object> dic)
        {
            SJException SJE = new SJException();
            Dictionary<string, string> pdic = new Dictionary<string, string>();
            pdic["gameid"] = "string";
            pdic["Platformid"] = "int";
            pdic["loginAccount"] = "string";
            pdic["password"] = "string";
            pdic["lastArea"] = "string";
            Dictionary<string, object> Param = getParam(Request, pdic);
            Dictionary<int,Dictionary<string,string>> SQLResult;
            string pass = (string)Param["password"];
            Tools.tools.Encryption(ref pass);
            pdic["password"] = "";
            if (Tools.DBOper.SpecialWordFiltering(Param) == false)
            {
                SJE.code = 1;
                SJE.Str = "非法用户利用字符串传递敏感字段,试图修改数据库";
                Tools.LoginFile.Write(SJE.Str);
                throw SJE;
            }
            string formatprve = "select top 1 * from Account";
            string cmd = string.Format("{0} where loginAccount='{1}' and password='{2}' and gameid='{3}'", formatprve, Param["loginAccount"], pass, Param["gameid"]);
            if (Tools.DBOper.select(cmd, ref SQLAccoutTableKeyList, out SQLResult) == false)
            {
                SJE.code = 2;
                SJE.Str = "请求数据库失败：具体查看日志";
                Tools.LoginFile.Write(SJE.Str);
                throw SJE;
            }
            if (SQLResult.Count == 0)
            {
                SJE.code = 3;
                SJE.Str = "该玩家不存在";
                throw SJE;
            }
            SQLResult[0]["password"] = null;
            dic["Data"] = SQLResult[0];
            /**/
            Dictionary<string,object> InPut = new Dictionary<string,object>();
            Dictionary<string,string> outformat = new Dictionary<string,string>();
            Dictionary<string,object> outPut;
            InPut["@AccountId"] = SQLResult[0]["AccountId"];
            InPut["@lastArea"] = Param["lastArea"];
            InPut["@Platformid"] = Param["Platformid"];
            InPut["@szToken"] = "siduyfbxdfvsdfgsdfdsghsdjfosiduygtfvy";
            outformat["@result"] = "int";
            Tools.DBOper.UserSqlStoredProc(InPut,ref outformat,out outPut,"sp_AccountLogin");
            if( outPut["@result"].ToString() != "0")
            {
                dic["Data"] = null;
                SJE.code = 4;
                SJE.Str = "最后更新玩家信息失败";
                dic["SQLProcResult"] = outPut["@result"];
                throw SJE;
            }
            return;
        }
        public static void Register(HttpRequest Request, ref Dictionary<string, object> dic)
        {
            SJException SJE = new SJException();
            Dictionary<string, string> pdic = new Dictionary<string, string>();
            pdic["gameid"] = "string";
            pdic["Platformid"] = "int";
            pdic["loginAccount"] = "string";
            pdic["password"] = "string";
            pdic["createArea"] = "string";
            pdic["imei"] = "string";
            pdic["VerificationCode"] = "string";
            Dictionary<string, object> Param = getParam(Request, pdic);
            if (Param["loginAccount"].ToString().Length < 2){
                SJE.code = 1;
                SJE.Str = "注册账号长度错误";
                throw SJE;
            }
            if (Param["password"].ToString().Length < 6 || Param["password"].ToString().Length > 50)
            {
                SJE.code = 2;
                SJE.Str = "密码设置长度错误";
                throw SJE;
            }
            long tel = 0;
            if (Tools.tools.IsHandset(Param["loginAccount"].ToString())){
                tel = Convert.ToInt64((string)Param["loginAccount"]);
            }
            Param["tel"] = tel;
            Param["sex"] = true;
            string pass = (string)Param["password"];
            Tools.tools.Encryption(ref pass);
            Param["password"] = pass;

            Dictionary<string,string> outformat = new Dictionary<string,string>();
            outformat["@AccountId"] = "string";
            outformat["@result"] = "int";
            Dictionary<string,object> outPut;
            if (false == Tools.DBOper.UserSqlStoredProc(Param,ref outformat,out outPut,"sp_AccountRegister")){
                SJE.code = 3;
                SJE.Str = "调用注册的存储过程失败";
                throw SJE;
            }
            if(outPut["@result"].ToString() != "0"){
                SJE.code = 4;
                SJE.Str = "执行注册的存储过程失败";
                dic["SQLProcResult"] = outPut["@result"];
                throw SJE;
            }
        }
        public static void CreateVerificationCode(HttpRequest Request, ref Dictionary<string, object> dic)
        {
            SJException SJE = new SJException();
            Dictionary<string, string> pdic = new Dictionary<string, string>();
            pdic["gameid"] = "string";
            pdic["Platformid"] = "int";
            pdic["Tel"] = "string";
            pdic["password"] = "string";
            pdic["createArea"] = "string";
            pdic["imei"] = "string";
            pdic["VerificationCode"] = "string";
            var Param = getParam(Request, pdic);
            long tel = 0;
            if (Tools.tools.IsHandset(Param["Tel"].ToString()))
            {
                tel = Convert.ToInt64((string)Param["Tel"]);
                Param["tel"] = tel;
            }
            else
            {
                Param["tel"] = Param["imei"].ToString();
            }
            string verification = VerifcationCodeHandler.CreateVerifcationCode((string)Param["gameid"],Param["tel"].ToString(),1);
            if (tel == 0)
            {
                pdic["verification"] = verification;
                return;
            }
            else
            {

            }
        }
        public static void ModifyPass(HttpRequest Request, ref Dictionary<string, object> dic)
        {
            SJException sjh = new SJException();
            Dictionary<string, string> ParamType = new Dictionary<string, string>();
            ParamType["gameid"] = "string";
            ParamType["RegisterArea"] = "string";
            ParamType["RegisterTime"] = "string";
            ParamType["VerificationCode"] = "string";//验证码或者注册时候机器码
            ParamType["loginAccount"] = "string";
            ParamType["password"] = "string";
            Dictionary<string, object> Param = getParam(Request, ParamType);
            //首先验证验证码是否存在?


        }
    }
}