#include "stdafx.h"
#include "LoadIni.h"
#include <Windows.h>
#include "Tools.h"
#include <list>
LoadIni *LoadIni::IniData = NULL;
LoadIni::LoadIni()
{
	Map.clear();
}
LoadIni::~LoadIni()
{
	Map.clear();
}
bool LoadIni::Load(int argc, _TCHAR* argv[]){
	LogDebug("bool LoadIni::Load(int argc, _TCHAR* argv[]){");
	if (argc < 2){
		LogError("缺少命令行参数");
		return false;
	}
	std::string filepath = Tools::strW2C(argv[1]);
	FILE* fp = fopen(filepath.c_str(), "r");
	if (fp == NULL){
		LogError("打不开配置文件:%s",filepath.c_str());
		return false;
	}
	char buffer[1024] = { 0 };
	while (!feof(fp))
	{
		fgets(buffer, 1024, fp);
		if (strlen(buffer) > 3){
			LogDebug("Buffer = %s", buffer);
			if (buffer[0] == '/' && buffer[1] == '/')
				continue;
			if (buffer[0] == ':' && buffer[1] == ':')
				continue;
			std::string str = buffer;
			int pos = str.find("=");
			if (pos < 0)
				continue;
			if (pos == 0 || pos > str.length() - 2)
				continue;
			std::string key = str.substr(0, pos);
			std::string value = str.substr(pos + 1, str.length() - pos - 1);
			if (value[value.length() - 1] == '\n')
				value = value.substr(0, value.length() - 1);
			if (Map.find(key) != Map.end())
				continue;
			Map[key] = value;
			LogDebug("key = %s,value = %s", key.c_str(), value.c_str());
		}
	}
	return false;
}
std::string& LoadIni::operator[](const std::string key){
	if (Map.find(key) == Map.end()){
		Map[key] = "";
	}
	return Map[key];
}
bool LoadIni::CheckIni(){
	//主要lua运行初始文件  作为服务器端口   是否使用数据库   服务器id  web服务器地址
	std::list<std::string> slist = {"LuaWork","ServerPort","IsUserDB","ServerId","WebUrl"};
	for each (auto var in slist)
	{
		if (this->Map.find(var) == Map.end()){
			LogDebug(" 缺少配置ini %s", var.c_str());
			return false;
		}
	}
	//主要lua服务 进程初始启动  进程最后扫尾以及bug提交版本更新系列
	std::list<std::string> filepath = { "LuaWork","LuaStart","LuaEnd" };
	FILE *fp = NULL;
	for each (auto var in filepath)
	{
		if (this->Map.find(var) != Map.end()){
			fp = fopen(Map[var].c_str(), "r");
			if (fp == NULL)
			{
				LogDebug("配置%s的文件路径不存在",Map[var].c_str());
				return false;
			}
			fclose(fp);
		}
	}
	std::list<std::string> sbool = { "IsUserDB" };
	for each (auto var in sbool)
	{
		if (this->Map.find(var) != Map.end()){
			Map[var] = Map[var] == "true" ? "true" : "false";
		}
	}
	if (Map["IsUserDB"] == "true"){
		auto iter = Map.find("DBFileConnectCount");
		if (iter == Map.end()){
			LogDebug("可以使用数据库配置下 缺少DBFileConnectCount");
			return false;
		}
	}
	return true;
}
//aaa["ff"] = ""