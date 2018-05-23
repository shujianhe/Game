using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Security;
using System.Security.Cryptography;
namespace SJHanHaiGameDataWeb.Tools
{
    public class tools
    {
        private static bool Debug = true;
        private string AllChar = "qwsdfreacvgtyhnparam.KeyContainerName=SecretKey;juikolpmnxsqawerivbhjxPOSDFXCVNJUIWEXCVNJKZQYUTEZSDFRTYHFDFMNVZXCASDFGRWSERIJNVMKL,{}!@#$%^&*UIHSDFHU&*90-234415740.6-+~2SDF*/";
        private static byte[] bip;
        static tools()
        {
            string Key = "314159265310653101182012320118126281261810278101281012310122901231012190123106535897932384626431383279586280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867";
            bip = System.Text.Encoding.Default.GetBytes(Key);
        }
        public static bool Encryption(string express, string SecretKey,out string pass)
        {
            if (Debug == true)
            {
                pass = express;
                return true;
            }
            //用时间当作掺盐
            try
            {
                string time = DateTime.Now.ToString();
                express = time.Length + "t" + time + "T" + express.Length + "D:" + express;
                byte[] byteexpress = System.Text.Encoding.Default.GetBytes(express);
                byte[] byteKey = System.Text.Encoding.Default.GetBytes(SecretKey);
                string result = "";
                int index = 0;
                Random pl = new Random(0);
                string debugstr = "";
                for (int i = 0; i < byteexpress.Count(); i++)
                {
                    index = (byteexpress.Count() - i) % SecretKey.Count();
                    debugstr = debugstr + byteexpress[i] + "|";
                    switch (i % 6)
                    {
                        case 0:
                            {
                                result = result + "a" + ((byteexpress[i] + byteKey[index]) / (byte.MaxValue + 1));
                                result = result + "%" + ((byteexpress[i] + byteKey[index]) % (byte.MaxValue + 1)) + "e";
                            } break;
                        case 1:
                            {
                                result = result + "b" + ((byteexpress[i] * byteKey[index]) / (byte.MaxValue + 1));
                                result = result + "%" + ((byteexpress[i] * byteKey[index]) % (byte.MaxValue + 1)) + "e";
                            } break;
                        case 2:
                            {
                                result = result + "c" + i % 10;
                                result = result + "%" + byteexpress[i] + "e";
                            } break;
                        case 3:
                            {
                                int pkv = pl.Next() % 3000;
                                result = result + "d" + (int)(byteexpress[i] + pkv);
                                result = result + "%" + pkv + "e";
                            } break;
                        case 4:
                            {
                                int pkv = pl.Next() % 8000;
                                result = result + "f" + (int)(byteexpress[i] * pkv);
                                result = result + "%" + pkv + "e";
                            } break;
                        case 5:
                            {
                                int pkv = pl.Next() % 78456;
                                result = result + "g" + (int)(byteexpress[i] * pkv / byteKey[index]);
                                result = result + "%" + pkv + "e";
                            } break;
                    }
                }
                pass = byteexpress.Count() + result;
            }
            catch (Exception ex)
            {
                pass = "";
                return false;
            }
            return true;
        }
        //解密
        public static bool Decrypt(string express, string SecretKey,out string str)
        {
            if (Debug == true)
            {
                str = express;
                return true;
            }
            str = "";
            try
            {
                byte[] byteKey = System.Text.Encoding.Default.GetBytes(SecretKey);
                int tempindex = express.IndexOf("a");
                if (tempindex < 0)
                {
                    str = "不是合法的加密结果字符串";
                    return false;
                }
                int len = Convert.ToInt32(express.Substring(0, tempindex));
                byte[] byteexpress = new byte[len];
                express = express.Substring(tempindex, express.Length - tempindex);
                string Result = "";
                string tempstr = "";
                tempindex = express.IndexOf("e");
                string debugstr = "";
                while (tempindex > -1)
                {
                    tempstr = express.Substring(0, tempindex);
                    express = express.Substring(tempindex + 1, express.Length - tempindex - 1);
                    int tempkeyindex = (len - Result.Length) % SecretKey.Count();
                    int sum = 0;
                    tempindex = tempstr.IndexOf("%");
                    if (tempindex < 1)
                    {
                        str = "不是合法的加密结果字符串";
                        return false;
                    }
                    string ss = tempstr.Substring(1, tempindex - 1);
                    int num1 = 0;
                    if (tempstr[0] != 'c')
                    {
                        num1 = Convert.ToInt32(ss);
                    }
                    int num2 = Convert.ToInt32(tempstr.Substring(tempindex + 1, tempstr.Length - tempindex - 1));
                    switch (tempstr[0])
                    {
                        case 'a':
                            {
                                sum = num1 * (1 + byte.MaxValue) + num2;
                                sum = sum - byteKey[tempkeyindex];
                            }
                            break;
                        case 'b':
                            {
                                sum = (num1 * (byte.MaxValue + 1) + num2) / byteKey[tempkeyindex];
                            }
                            break;
                        case 'c':
                            {
                                sum = num2;
                            }
                            break;
                        case 'd':
                            {
                                sum = num1 - num2;
                            } break;
                        case 'f':
                            {
                                sum = num1 / num2;
                            }
                            break;
                        case 'g':
                            {
                                sum = num1 * byteKey[tempkeyindex] / num2 +1;
                            } break;
                        default:
                            {
                                str = "不是合法的加密结果字符串异常标记";
                            }
                            return false;
                    }
                    debugstr = debugstr + sum + "|";
                    tempindex = express.IndexOf("e");
                    byteexpress[Result.Length] = (byte)sum;
                    Result = Result + "2";
                }
                Result = System.Text.Encoding.Default.GetString(byteexpress);
                tempindex = Result.IndexOf("T");
                if (tempindex < 1)
                    return false;
                int templenindex = Result.IndexOf("D:");
                tempstr = Result.Substring(tempindex + 1, templenindex - tempindex - 1);
                len = Convert.ToInt32(tempstr);
                str = Result.Substring(2 + templenindex, len);
            }
            catch (Exception ex)
            {
                str = ex.ToString();
                return false;
            }            
            return true;
        }
        public static string getRandStr(int len)
        {
            if (len <= 0)
                return "";
            else
            {
                string result = "";
                tools t = new tools();
                Random r = new Random();
                int MaxLen = t.AllChar.Length;
                for (int i = 0; i < len; i++)
                {
                    result += t.AllChar[r.Next()%MaxLen];
                }
                return result;
            }
        }
        public static long getTimeStamp()
        {
            TimeSpan ts = DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, 0);
            return Convert.ToInt64(ts.TotalSeconds);
        }
        public static void Encryption(ref string pass)
        {
            
            byte[] bytepass = System.Text.Encoding.Default.GetBytes(pass);
            pass = "";
            for (int i = 0; i < bytepass.Length; i++)
            {
                byte b = bip[bytepass[i]];
                pass = pass + Convert.ToInt64(b);
            }
        }
        public static void Encryption(ref object pass)
        {
            string s = pass.ToString();
            Encryption(ref s);
            pass = s;
        }
        public static void update2PassKey()
        {
            //需要从数据库中获取
            Func<object> hand = delegate()
            {
                List<string> list = new List<string>();
                list.Add("gameid");
                list.Add("url");
                list.Add("Platformid");
                list.Add("Clientversion");
                list.Add("Payway");
                list.Add("status");
                list.Add("PassKey");
                Dictionary<int, Dictionary<string, string>> SQLDBServerInfo;
                bool dbselectres = DBOper.select("SELECT * FROM WebServerInfo", ref list, out SQLDBServerInfo);
                if (dbselectres == false)
                    return null;
                else if (SQLDBServerInfo.Count < 1)
                    return null;
                else
                    return SQLDBServerInfo;
            };
            Dictionary<int, Dictionary<string, string>> WebServerInfo = (Dictionary<int, Dictionary<string, string>>)InstanceData.get("WebServerInfo", hand);
            if (WebServerInfo == null)
            {
                return;
            }
            string key = WebServerInfo[0]["PassKey"];
            if(key != null && key.Length > 255)
            {
                lock (bip)
                {
                    bip = System.Text.Encoding.Default.GetBytes(key);
                }
            }
        }
        public static bool IsHandset(string str_handset)
        {
            return System.Text.RegularExpressions.Regex.IsMatch(str_handset, @"^[1]+[3,5]+\d{9}");
        }
    }
}