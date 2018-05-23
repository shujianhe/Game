#include "stdafx.h"
#include "LuaHelp.h"
#include "LoadIni.h"
#include "Tools.h"
#include "UVSocketManage.h"
using namespace userlua;
LuaHelp* LuaHelp::LH = NULL;
LuaHelp::LuaHelp()
{
	L = NULL;
}
LuaHelp::~LuaHelp()
{
}
bool LuaHelp::init(std::function<void(lua_State*)> initBack){
	LogDebug("init Luahelp");
	L = luaL_newstate();
	luaL_openlibs(L);
	auto pmap = LoadIni::GetInstance()->getMap();
	lua_getglobal(L, "_G");
	lua_pushstring(L, "_WG");
	lua_newtable(L);
	lua_pushstring(L, "Cpp__Ini__Config");
	lua_newtable(L);
	for each (auto var in pmap){
		lua_pushstring(L, var.first.c_str());
		lua_pushstring(L, var.second.c_str());
		lua_settable(L, -3);
	}
	lua_settable(L, -3);
	lua_settable(L, -3);
	initBack(L);
	return true;
}
bool LuaHelp::RunLuaFile(std::string stratFilePath){
	try{
		LogDebug("执行文件:%s",stratFilePath.c_str());
		luaL_loadfile(L, stratFilePath.c_str());
		if (lua_pcall(L, 0, 0, 0)){
			LogError("中途推出程序");
			return true;
		}
		LogInfo("执行完成");
		lua_close(L);
		this->Clear();
	}
	catch (void* d){
		printf("出现了崩溃地方");
	}
	return true;
}
bool LuaHelp::Clear(){
	useruv::UVSocketManage::Destroy();
	useruv::UVRecvDataQueue::Destroy();
	return true;
}