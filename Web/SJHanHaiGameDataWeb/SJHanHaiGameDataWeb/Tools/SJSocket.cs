using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.IO;
using System.Net;
using System.Net.Sockets;
namespace SJHanHaiGameDataWeb.Tools
{
    public class SJSocket
    {
        static int recvDataLen = 1024*10;
        static Action<string> LogHandler;
        static string DefaultIp;
        static int DefaultPort;
        static SJSocket()
        {
            LogHandler = delegate(string s)
            {
                LoginFile.Write(s);
            };
            Func<object> NilHandle = delegate()
            {
                List<string> list = new List<string>();
                list.Add("gameid");
                list.Add("ServerIp");
                list.Add("ServerProt");
                list.Add("ServerName");
                Dictionary<int,Dictionary<string,string>> SQLDBServerInfo;
                bool dbselectres = DBOper.select("SELECT top 1 * FROM DBServerInfo", ref list, out SQLDBServerInfo);
                if (dbselectres == false)
                    return null;
                else if (SQLDBServerInfo.Count < 1)
                    return null;
                else
                    return SQLDBServerInfo[0];
            };
            Object ODBServerInfo = Tools.InstanceData.get("DBServerInfo", NilHandle);
            if (null == ODBServerInfo)
            {
                throw new Exception("不能获取到ip和port");
            }
            Dictionary<string, string> DBServerInfo = ODBServerInfo as Dictionary<string, string>;
            DefaultIp = DBServerInfo["ServerIp"];
            DefaultPort = Convert.ToInt32(DBServerInfo["ServerProt"]);
        }
        public static bool sendTo(string ip, int port, string jsondata)
        {
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                socket.Connect(ip, port);
                SJNetMessage sjnm = new SJNetMessage(jsondata);
                socket.Send(sjnm.Data);
                socket.Close();
                return true;
            }
            catch(Exception ex)
            {
                socket.Close();
                LogHandler("发送失败:ip = "+ip+" port = "+port+" data = "+jsondata+"  错误:"+ex.ToString());
            }
            return false;
        }
        public static bool sendToRecv(string ip, int port,ref string jsondata)
        {
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                socket.Connect(ip, port);
                SJNetMessage sjnm = new SJNetMessage(jsondata);
                socket.Send(sjnm.Data);
                byte[] bjsondata = System.Text.Encoding.Unicode.GetBytes(jsondata);
                socket.Send(bjsondata);
                byte[] recvData = new byte[recvDataLen];
                socket.Receive(recvData);
                SJNetMessage recvsjnm = new SJNetMessage(recvData);
                Dictionary<string,object> result = recvsjnm.GetMsg();
                if (result == null)
                {
                    LogHandler("接收到的数据处理失败");
                    return false;
                }
                LogHandler("接收到的数据:" + result.ToString());
                jsondata = result["value"] as string;
                socket.Close();
                return true;
            }
            catch (Exception ex)
            {
                socket.Close();
                LogHandler("发送失败:ip = " + ip + " port = " + port + " data = " + jsondata + "  错误:" + ex.ToString());
            }
            return false;
        }
        public static bool sendTo(string jsondata)
        {
            return sendTo(DefaultIp, DefaultPort, jsondata);
        }
        public static bool sendToRecv(ref string jsondata)
        {
            return sendToRecv(DefaultIp, DefaultPort,ref jsondata);
        }
    }
}