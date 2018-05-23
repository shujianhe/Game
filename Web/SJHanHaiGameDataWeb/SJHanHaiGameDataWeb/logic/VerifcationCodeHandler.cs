using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace SJHanHaiGameDataWeb.logic
{
    public class VerifcationCodeHandler
    {
        public static bool IsIPhoneVerifcationCode(string s)
        {
            return true;
        }
        public static bool IsTelIEMIVerifcationCode(string gameid, string s)
        {
            return true;
        }
        public static string CreateVerifcationCode(string gameid, string AccountLogin, int Code)
        {
            SJException sje = new SJException();
            string verification = Tools.tools.getRandStr(7);
            Dictionary<string, string> format = new Dictionary<string, string>();
            format["@result"] = "int";
            Dictionary<string, object> Param = new Dictionary<string, object>();
            Param["Accountlogin"] = AccountLogin;
            Param["VerCode"] = verification;
            Param["Code"] = Code;
            Param["gameid"] = gameid;
            Dictionary<string, object> result;
            if (Tools.DBOper.UserSqlStoredProc(Param, ref format, out result, "") == false)
            {
                sje.code = -3;
                sje.Str = "执行存储过程失败";
                throw sje;
            }
            if (result["@result"].ToString() != "0" ){
                sje.code = -4;
                sje.Str = "执行存储过程中失败";
                throw sje;
            }
            return verification;
        }
        
    }
}