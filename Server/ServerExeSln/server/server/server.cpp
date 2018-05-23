// server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <ctime>
#pragma comment(lib,"ws2_32.lib")
#include "Tools.h"
#include "LoadIni.h"
#include "LuaHelp.h"
#include "UVSocketManage.h"
#include "TimeManage.h"
using userlua::LuaHelp;
static int MyG_Net_Server(lua_State* L){
	int port = lua_tonumber(L, 2);
	int hand = useruv::UVSocketManage::getInstance()->Server(port);
	if (hand > 0)
	{
		lua_pushnumber(L, hand);
		return 1;
	}
	return 0;
}
static int MyG_Net_Connect(lua_State* L){
	std::string ip = lua_tostring(L, 2);
	int port = lua_tonumber(L, 3);
	int hand = useruv::UVSocketManage::getInstance()->connect(ip, port);
	if (hand > 0){
		lua_pushnumber(L, hand);
		return 1;
	}
	return 0;
}
static int MyG_Net_Send(lua_State* L){
	int hand = lua_tonumber(L, 2);
	char *data = (char*)lua_tostring(L, 3);
	int len = lua_tonumber(L, 4);
	lua_pushboolean(L,useruv::UVSocketManage::getInstance()->Send(hand,data,len));
	return 1;
}
static int MyG_Net_Find(lua_State* L){
	char *ip = (char*)lua_tostring(L, 2);
	int port = lua_tonumber(L, 3);
	int hand = useruv::UVSocketManage::getInstance()->Find(ip, port);
	if (hand <= 0)
		lua_pushnil(L);
	lua_pushnumber(L, hand);
	return 1;
}
static int MyG_Net_Close(lua_State* L){
	int hand = lua_tonumber(L, 2);
	useruv::UVSocketManage::getInstance()->close(hand);
	return 0;
}
static int MyG_Net_GetMsg(lua_State* L){
	bool IsOk = false;
	int retsult = 0;
	useruv::UVRecvDataQueue::getInstance()->pop([&](int hand, int len, int st, int rt, char* data){
		IsOk = true;
		//lua_newtable(L);

		//lua_pushstring(L, "hand");
		lua_pushnumber(L, hand);
		//lua_settable(L, -3);

		//lua_pushstring(L, "len");
		lua_pushnumber(L, len);
		//lua_settable(L, -3);

		//lua_pushstring(L, "sendtime");
		lua_pushnumber(L, st);
		//lua_settable(L, -3);

		//lua_pushstring(L, "recvtime");
		lua_pushnumber(L, rt);
		//lua_settable(L, -3);

		//lua_pushstring(L, "data");
		std::string sdata = data;
		lua_pushstring(L, sdata.c_str());
		//lua_settable(L, -3);
		//lua_settable(L, -2);
		retsult = 5;
		//LogDebug("收的内容是:%d %d %d %d %s",hand,len,st,rt,data);
	});
	if (IsOk == false){
		retsult = 1;
		lua_pushnil(L);
		//LogDebug("收到空内容");
	}
	return retsult;
}
static int MyG_Net_ClearMsg(lua_State* L)
{
	LogDebug("回收所有未处理包开始");
	useruv::UVRecvDataQueue::Destroy();
	LogDebug("回收所有未处理包结束");
	return 0;
}
static int MyG_Net_ClearSocket(lua_State *L){
	LogDebug("回收所有网络资源开始");
	useruv::UVSocketManage::Destroy();
	LogDebug("回收所有网络资源完成");
	return 0;
}
static int MyG_Time_Update(lua_State* L){
	TimeManage::getInstance()->Update();
	return 0;
}
static int MyG_Time_SetFps(lua_State* L){
	int Fps = lua_tonumber(L, 2);
	lua_pushboolean(L,TimeManage::getInstance()->SetFps(Fps));
	return 1;
}
static int MyG_Time_GetFps(lua_State* L){
	lua_pushnumber(L, TimeManage::getInstance()->GetFps());
	return 1;
}
static int MyG_Time_GetCurCpuTime(lua_State* L){
	lua_pushnumber(L, TimeManage::getInstance()->GetCurCpuTime());
	return 1;
}
static int MyG_Time_getAllCallBack(lua_State* L){
	lua_newtable(L);
	auto backlist = TimeManage::getInstance()->getAllCallBack();
	int i = 0;
	for (auto iter = backlist.begin(); iter != backlist.end(); iter++){
		i++;
		lua_pushnumber(L, i);
		lua_pushstring(L, iter->c_str());
		lua_settable(L, -3);
	}
	return 1;
}
static int MyG_Time_UnScheduler(lua_State* L){
	std::string key = lua_tostring(L, 2);
	TimeManage::getInstance()->UnScheduler(key);
	return 0;
}
static int MyG_Time_addScheduler(lua_State* L){
	double len = lua_tonumber(L, 2);
	bool loop = lua_toboolean(L, 3);
	std::string key = TimeManage::getInstance()->addScheduler(len,loop);
	lua_pushstring(L, key.c_str());
	return 1;
}
void onRegister(lua_State* L){
	lua_getglobal(L, "_G");
	lua_pushstring(L, "_MyG");
	lua_newtable(L);
	lua_pushstring(L, "Net");
	lua_newtable(L);

	lua_pushstring(L, "Server");
	lua_pushcfunction(L, MyG_Net_Server);
	lua_settable(L, -3);

	lua_pushstring(L, "Connect");
	lua_pushcfunction(L, MyG_Net_Connect);
	lua_settable(L, -3);

	lua_pushstring(L, "Send");
	lua_pushcfunction(L, MyG_Net_Send);
	lua_settable(L, -3);

	lua_pushstring(L, "Find");
	lua_pushcfunction(L, MyG_Net_Find);
	lua_settable(L, -3);

	lua_pushstring(L, "Close");
	lua_pushcfunction(L, MyG_Net_Close);
	lua_settable(L, -3);

	lua_pushstring(L, "getmsg");
	lua_pushcfunction(L, MyG_Net_GetMsg);
	lua_settable(L, -3);

	lua_pushstring(L, "ClearMsg");
	lua_pushcfunction(L, MyG_Net_ClearMsg);
	lua_settable(L, -3);

	lua_pushstring(L, "ClearSocket");
	lua_pushcfunction(L, MyG_Net_ClearSocket);
	lua_settable(L, -3);

	lua_settable(L, -3);//End Net

	lua_pushstring(L, "Time");
	lua_newtable(L);
	
	lua_pushstring(L, "Update");
	lua_pushcfunction(L, MyG_Time_Update);
	lua_settable(L, -3);

	lua_pushstring(L, "SetFps");
	lua_pushcfunction(L, MyG_Time_SetFps);
	lua_settable(L, -3);

	lua_pushstring(L, "GetFps");
	lua_pushcfunction(L, MyG_Time_GetFps);
	lua_settable(L, -3);

	lua_pushstring(L, "GetCurCpuTime");
	lua_pushcfunction(L, MyG_Time_GetCurCpuTime);
	lua_settable(L, -3);

	lua_pushstring(L, "getAllCallBack");
	lua_pushcfunction(L, MyG_Time_getAllCallBack);
	lua_settable(L, -3);

	lua_pushstring(L, "addScheduler");
	lua_pushcfunction(L, MyG_Time_addScheduler);
	lua_settable(L, -3);

	lua_pushstring(L, "UnScheduler");
	lua_pushcfunction(L, MyG_Time_UnScheduler);
	lua_settable(L, -3);

	lua_settable(L, -3);//End Time

	lua_settable(L, -3);
}
int Lua_Test_GetTable(lua_State *L)
{
	lua_newtable(L);
	for (int i = 0; i < 5; i++)
	{
		lua_pushnumber(L, i);
		lua_pushnumber(L, i * 10);
		lua_settable(L, -3);
	}
	//lua_settable(L, -3);
	return 1;
}
void onRegister2(lua_State* L){
	lua_pushstring(L, "CppGetTable");
	lua_pushcfunction(L, Lua_Test_GetTable);
	lua_settable(L, -3);
}
void test(int argc, _TCHAR* argv[]){
	LoadIni::GetInstance()->Load(argc, argv);
	INI("test_1") = "天外飞仙_1";
	INI("test_2") = "天外飞仙_2";
	INI("test_3") = "天外飞仙_3";
	INI("test_4") = "天外飞仙_4";
	INI("test_5") = "天外飞仙_5";
	INI("test_6") = "天外飞仙_6";
	INI("test_7") = "天外飞仙_7";
	INI("test_8") = "天外飞仙_8";
	LuaHelp *lp = LuaHelp::getInstance();
	lp->init([](lua_State* L){
		onRegister(L);
	});
	lp->RunLuaFile(INI("LuaStart"));
	lp->init([](lua_State* L){
		onRegister2(L);
	});
	lp->RunLuaFile(INI("LuaWork"));
	lp->init();
	lp->RunLuaFile(INI("LuaEnd"));
}
void EndClear()
{
}
int _tmain(int argc, _TCHAR* argv[])
{
	test(argc, argv);
	return 0;
}