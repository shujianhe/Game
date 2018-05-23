using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data.Sql;
using System.Data.SqlClient;

namespace SJHanHaiGameDataWeb.Tools
{
    public class DBOper
    {
        private static DBOper instance;
        private static bool Debug = true;
        private static Func<string,int, bool> LogOutFunc;
        private static List<string> SpecialList;
        private static int SpecialTextMin;
        static DBOper() {
            LogOutFunc = delegate(string s,int level)
            {
                if (Debug == false && level == 0)
                    return true;
                return LoginFile.Write(s);
            };
            SpecialList = new List<string>();
            SpecialList.Add(" where ");
            SpecialList.Add("update ");
            SpecialList.Add("select ");
            SpecialList.Add("insert ");
            SpecialList.Add(" form ");
            SpecialList.Add("create ");
            SpecialList.Add("drop ");
            SpecialList.Add("alter ");
            SpecialList.Add(" and ");
            SpecialList.Add(" or ");
            SpecialList.Add(" not ");
            SpecialList.Add(" join ");
            SpecialList.Add(" order ");
            SpecialTextMin = 10000;
            foreach (var item in SpecialList)
            {
                SpecialTextMin = Math.Min(SpecialTextMin, item.Length);
            }
        }
        public static string getConStr()
        {
            return System.Configuration.ConfigurationManager.AppSettings["DBConStr"];
        }
        //检查传入参数有没有数据的关键字，防止sql注入攻击
        public static bool SpecialWordFiltering(string Text)
        {
            if (Text.Length < SpecialTextMin)
            {
                return true;
            }
            foreach (var item in SpecialList)
            {
                if (Text.Length >= item.Length)
                {
                    if( 0 < Text.IndexOf(item))
                        return false;
                }
            }
            return true;
        }
        public static bool SpecialWordFiltering(string[] Texts)
        {
            foreach (var item in Texts)
            {
                if (false == SpecialWordFiltering(item))
                    return false;
            }
            return true;
        }
        public static bool SpecialWordFiltering(Dictionary<string, string> dic)
        {
            foreach (var item in dic)
            {
                if (false == SpecialWordFiltering(item.Value))
                    return false;
            }
            return true;
        }

        public static bool SpecialWordFiltering(Dictionary<int, string> dic)
        {
            foreach (var item in dic)
            {
                if (false == SpecialWordFiltering(item.Value))
                    return false;
            }
            return true;
        }

        public static bool SpecialWordFiltering(Dictionary<string, object> dic)
        {
            string s = "";
            Type t = s.GetType();
            foreach (var item in dic)
            {
                if (item.Value.GetType() == t)
                {
                    if (false == SpecialWordFiltering(item.Value as string))
                        return false;
                }
            }
            return true;
        }
        public static bool SpecialWordFiltering(Dictionary<int, object> dic)
        {
            string s = "";
            Type t = s.GetType();
            foreach (var item in dic)
            {
                if (item.GetType() == t)
                {
                    if (false == SpecialWordFiltering(item.Value as string))
                        return false;
                }
            }
            return true;
        }
        //查询  连接字符串 查询语句 需要查询结果的所有字段值 返回结果 二级map全是字符串
        public static bool select(string conStr,string cmdStr,ref List<string> keys ,out Dictionary<int, Dictionary<string, string>> result)
        {
            result = new Dictionary<int,Dictionary<string,string>>();
            SqlConnection sql = null;
            SqlCommand sqlcmd = null;
            SqlDataReader sdr = null;
            try
            {
                sql = new SqlConnection(conStr);
                sql.Open();
                LogOutFunc("select :"+cmdStr,0);
                sqlcmd = new SqlCommand(cmdStr, sql);
                sdr = sqlcmd.ExecuteReader();
                if (sdr.HasRows == false)
                {
                    sql.Close();
                    return true;//没有记录
                }
                int Index = 0;
                while (sdr.Read())
                {
                    result[Index] = new Dictionary<string,string>();
                    foreach(string item in keys){//获取所有记录
                        if (sdr[item] != null)
                        {
                            result[Index][item] = sdr[item].ToString();
                        }
                    }
                    Index = Index + 1;
                }
                sdr.Close();
                sql.Close();
                return true;
            }
            catch (Exception ex) {
                LogOutFunc("数据库查询语句失败:" + ex.ToString(),2);
                LogOutFunc("连接串:" + conStr,2);
                LogOutFunc("查询语句:" + cmdStr,2);
                if (sql != null)
                {
                    sql.Close();
                }
                if (sdr != null && (!sdr.IsClosed))
                {
                    sdr.Close();
                }
                return false;
            }
        }
        public static bool select(string conStr, string cmdStr, ref List<string> keys, out Dictionary<string, Dictionary<string, string>> result)
        {
            Dictionary<int, Dictionary<string, string>> tempDic;
            bool bresult = select(conStr, cmdStr, ref keys,out tempDic);
            result = new Dictionary<string, Dictionary<string, string>>();
            if (bresult)
            {
                foreach (KeyValuePair<int,Dictionary<string,string>> item in tempDic)
                {
                    result[item.Key.ToString()] = item.Value;
                }
            }
            return bresult;
        }
        public static bool select(string cmdStr, ref List<string> keys, out Dictionary<int, Dictionary<string, string>> result)
        {
            return select(getConStr(), cmdStr,ref keys,out result);
        }
        public static bool select(string cmdStr, ref List<string> keys, out Dictionary<string, Dictionary<string, string>> result)
        {
            return select(getConStr(), cmdStr, ref keys, out result);
        }
        public static int insert(string conStr, string tablename,Dictionary<string, string> value)
        {
            string cmdstr = "insert "+tablename+" ";
            string key = "(";
            string strvalue = "(";
            int result = -1;
            foreach (KeyValuePair<string,string> item in value)
            {
                key = key + item.Key + ",";
                strvalue = strvalue + item.Value + ",";
            }
            if (key == "(")
                return result;
            key = key.Substring(0, key.Length - 1) + ") ";
            strvalue = strvalue.Substring(0, strvalue.Length - 1) + ") ";
            cmdstr = cmdstr + " " + key + "values " + strvalue;
            SqlConnection sql = null;
            SqlCommand sqlcmd = null;
            try
            {
                sql = new SqlConnection(conStr);
                sql.Open();
                LogOutFunc("insert :" + cmdstr,0);
                sqlcmd = new SqlCommand(cmdstr,sql);
                result = sqlcmd.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                LogOutFunc("数据库插入语句失败:" + ex.ToString(), 2);
                LogOutFunc("连接串:" + conStr, 2);
                LogOutFunc("插入语句:" + cmdstr, 2);
                result = -2;
            }
            finally
            {
                if (sqlcmd != null)
                    sql.Close();
            }
            return result;
        }
        public static int insert(string tablename, Dictionary<string, string> value)
        {
            return insert(getConStr(), tablename, value);
        }
        public static int update(string conStr, string tablename, Dictionary<string, string> value, Dictionary<string, string> where)
        {
            int result = -1;
            string cmdstr = "update " + tablename + " set ";
            string setvalue = "";
            string setwhere = "";
            foreach (KeyValuePair<string,string> item in value)
            {
                setvalue = setvalue + item.Key + "=" + item.Value + ",";
            }
            if (setvalue == "")
            {
                return -2;
            }
            setvalue = setvalue.Substring(0, setvalue.Length - 1);
            foreach (KeyValuePair<string, string> item in where)
            {
                setwhere = setwhere + item.Key + "=" + item.Value + " and ";
            }
            if (setwhere == "")
            {
                return -3;
            }
            setwhere = setwhere.Substring(0, setwhere.Length - 4);
            cmdstr = cmdstr + setvalue+" where " + setwhere;
            SqlConnection sql = null;
            SqlCommand sqlcmd = null;
            try
            {
                sql = new SqlConnection(conStr);
                sql.Open();
                LogOutFunc("update :" + cmdstr,0);
                sqlcmd = new SqlCommand(cmdstr, sql);
                result = sqlcmd.ExecuteNonQuery();
            }
            catch (Exception ex)
            {
                LogOutFunc("数据库更新语句失败:" + ex.ToString(), 2);
                LogOutFunc("连接串:" + conStr, 2);
                LogOutFunc("更新语句:" + cmdstr, 2);
                result = -2;
            }
            finally
            {
                if (sqlcmd != null)
                    sql.Close();
            }
            return result;
        }
        public static int update(string tablename, Dictionary<string, string> value, Dictionary<string, string> where)
        {
            return update(getConStr(), tablename, value, where);
        }
        public static bool UserSqlStoredProc(string conStr, Dictionary<string, object> Input, ref Dictionary<string, string> resultformat,out Dictionary<string,object> result, string StoreProcName)
        {
            SqlCommand cmd = new SqlCommand();
            cmd.CommandText = StoreProcName;
            cmd.CommandType = System.Data.CommandType.StoredProcedure;
            //添加输入参数
            foreach (KeyValuePair<string, object> kvpInput in Input)
            {
                cmd.Parameters.AddWithValue(kvpInput.Key, kvpInput.Value);
            }

            //添加输出参数
            foreach (KeyValuePair<string, string> kvpOut in resultformat)
            {
                SqlParameter param = null;
                switch (kvpOut.Value)
                {
                    case "int":
                        param = new SqlParameter(kvpOut.Key, 0);
                        break;
                    case "int32":
                        param = new SqlParameter(kvpOut.Key, 0);
                        break;
                    case "int64":
                        param = new SqlParameter(kvpOut.Key, 0);
                        break;
                    case "int16":
                        param = new SqlParameter(kvpOut.Key, 0);
                        break;
                    case "bool":
                        param = new SqlParameter(kvpOut.Key, false);
                        break;
                    case "Boolean":
                        param = new SqlParameter(kvpOut.Key, false);
                        break;
                    case "double":
                        param = new SqlParameter(kvpOut.Key, 0);
                        break;
                    case "string":
                        param = new SqlParameter(kvpOut.Key, "");
                        break;
                    default:
                        {
                            result = null;
                            return false;
                        }
                }
                param.Direction = System.Data.ParameterDirection.Output;
                param.Size = 64;
                param.DbType = System.Data.DbType.String;
                cmd.Parameters.Add(param);
            }
            SqlConnection sConn = new SqlConnection(conStr);
            cmd.Connection = sConn;
            //执行数据库操作命令
            using (sConn)
            {
                sConn.Open();
                cmd.ExecuteNonQuery();
                sConn.Close();
                sConn.Dispose();
            }

            //返回存储过程的输出参数值
            result = new Dictionary<string, object>();
            foreach (KeyValuePair<string, string> kvpOut in resultformat)
            {
                result[kvpOut.Key] = cmd.Parameters[kvpOut.Key].Value;
            }
            return true;
        }
        public static bool UserSqlStoredProc(Dictionary<string, object> Input, ref Dictionary<string, string> resultformat, out Dictionary<string, object> result, string StoreProcName)
        {
            return UserSqlStoredProc(getConStr(),Input,ref resultformat, out result, StoreProcName);
        }
    }
}