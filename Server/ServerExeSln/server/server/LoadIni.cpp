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
		LogError("ȱ�������в���");
		return false;
	}
	std::string filepath = Tools::strW2C(argv[1]);
	FILE* fp = fopen(filepath.c_str(), "r");
	if (fp == NULL){
		LogError("�򲻿������ļ�:%s",filepath.c_str());
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
	//��Ҫlua���г�ʼ�ļ�  ��Ϊ�������˿�   �Ƿ�ʹ�����ݿ�   ������id  web��������ַ
	std::list<std::string> slist = {"LuaWork","ServerPort","IsUserDB","ServerId","WebUrl"};
	for each (auto var in slist)
	{
		if (this->Map.find(var) == Map.end()){
			LogDebug(" ȱ������ini %s", var.c_str());
			return false;
		}
	}
	//��Ҫlua���� ���̳�ʼ����  �������ɨβ�Լ�bug�ύ�汾����ϵ��
	std::list<std::string> filepath = { "LuaWork","LuaStart","LuaEnd" };
	FILE *fp = NULL;
	for each (auto var in filepath)
	{
		if (this->Map.find(var) != Map.end()){
			fp = fopen(Map[var].c_str(), "r");
			if (fp == NULL)
			{
				LogDebug("����%s���ļ�·��������",Map[var].c_str());
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
			LogDebug("����ʹ�����ݿ������� ȱ��DBFileConnectCount");
			return false;
		}
	}
	return true;
}
//aaa["ff"] = ""