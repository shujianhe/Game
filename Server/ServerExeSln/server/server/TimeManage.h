#pragma once
#include <Windows.h>
#include <ctime>
#include <iostream>
#include <map>
#include <list>
#include <sstream>
class TimeEvent
{
public:
	std::string backKey;//用于回调的key
	double timelen;//定时器长度
	bool LoopFlg;//循环标记
	double startT;//起始时间
	double endT;//结束时间
	bool CallBackFlg;//回调标记
	//len 毫秒
	TimeEvent(std::string key, double len, bool loop);
	void Update(double len);
};
class TimeManage
{
private:
	static TimeManage* TM;
	LARGE_INTEGER qe,bg,ed;
	int FPS;
	std::map<std::string, TimeEvent*> timelist;
	std::list<std::string> backkeyslist;
private:
	TimeManage();
	~TimeManage();
public:
	static TimeManage* getInstance(){
		TM = TM ? TM : new TimeManage();
		return TM;
	}
	static void Destroy(){
		if (TM){
			delete TM;
		}
		TM = NULL;
	}
	void Update();
	inline bool SetFps(int fps){
		fps = fps - fps % 1;
		if (fps <= 0 || fps > 60)
			return false;
		FPS = fps;
		return true;
	}
	inline int GetFps(){
		return FPS;
	}
	inline double GetCurCpuTime(){
		QueryPerformanceCounter(&this->ed);
		double r = (double)(ed.QuadPart / qe.QuadPart);
		if (r < 0){
			r = 0 - r;
		}
		return r;
	}
	inline std::list<std::string> &getAllCallBack()
	{
		for each (auto var in backkeyslist)
		{
			auto Iter = timelist.find(var);
			if (Iter != timelist.end()){
				Iter->second->CallBackFlg = false;
			}
		}
		return this->backkeyslist;
	}
	inline std::string addScheduler(double len = -1,bool loop = false){
		std::string key = "";
		std::stringstream ss;
		TimeEvent *te = new TimeEvent("", len, loop);
		do
		{
			ss << "Time:" << GetCurCpuTime() << "Addr:" << (int)te;
			ss >> key;
		} while (timelist.find(key) != timelist.end());
		te->backKey = key;
		timelist[key] = te;
		return key;
	}
	inline void UnScheduler(std::string key){
		auto iter = timelist.find(key);
		if (iter != timelist.end())
		{
			delete iter->second;
			iter->second = NULL;
			timelist.erase(iter);
		}
	}
};