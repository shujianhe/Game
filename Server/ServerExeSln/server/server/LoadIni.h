#pragma once
#include <iostream>
#include <string>
#include <map>
#include <list>
class LoadIni
{
private:
	static LoadIni *IniData;
	std::map<std::string, std::string> Map;
private:
	LoadIni();
	~LoadIni();
	bool CheckIni();//检查配置完整性
public:
	bool Load(int argc, _TCHAR* argv[]);
	std::string& operator[](const std::string key);
	static LoadIni *GetInstance(){
		IniData = IniData ? IniData : new LoadIni();
		return IniData;
	}
	static void Destory(){
		if (IniData)
			delete IniData;
		IniData = NULL;
	}
	inline std::map<std::string, std::string>& getMap(){
		return Map;
	}
};
#define INI(key) (*LoadIni::GetInstance())[key]