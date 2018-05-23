#include "stdafx.h"
#include "TimeManage.h"
#include <profileapi.h>
TimeManage* TimeManage::TM = NULL;
TimeManage::TimeManage() :FPS(10)
{
	QueryPerformanceFrequency(&this->qe);
	this->bg.QuadPart = 0;
	this->ed.QuadPart = 0;
	timelist.clear();
	backkeyslist.clear();
}
TimeManage::~TimeManage()
{
	for each (auto var in timelist)
	{
		delete var.second;
		var.second = NULL;
	}
	timelist.clear();
}
void TimeManage::Update(){
	//更新定时器所有对象状态
	backkeyslist.clear();
	double Interval = (double)1 / this->FPS;
	double len = 0;
	if (bg.QuadPart == 0){
		len = Interval;
	}
	else{
		QueryPerformanceCounter(&this->ed);
		len = (ed.QuadPart - bg.QuadPart) / qe.QuadPart;
		len = Interval - len;
		if (len <= 0)
			len = Interval;
	}
	for each (auto var in timelist)
	{
		if (var.second->timelen == -1){
			var.second->CallBackFlg = true;
		}
		else{
			var.second->Update(len);
		}
		if (var.second->CallBackFlg){
			backkeyslist.push_back(var.first);
		}
	}
	for (auto iter = backkeyslist.begin(); iter != backkeyslist.end(); iter++)
	{
		if (timelist[*iter]->LoopFlg == false){
			delete timelist[*iter];
			timelist[*iter] = NULL;
			timelist.erase(timelist.find(*iter));
		}
	}
	if (Interval >= len){
		QueryPerformanceCounter(&this->ed);
		len = (ed.QuadPart - bg.QuadPart) / qe.QuadPart;
		len = Interval - len;
		if (len > 0)
			::Sleep(len * 1000);
		else
			::Sleep(Interval * 1000);
	}
	QueryPerformanceCounter(&this->bg);
	return;
}
TimeEvent::TimeEvent(std::string key, double len, bool loop){
	backKey = key;
	timelen = len;
	LoopFlg = loop;
	startT = TimeManage::getInstance()->GetCurCpuTime();
	endT = startT + timelen;
	CallBackFlg = false;
}
void TimeEvent::Update(double len){
	if (CallBackFlg == true)
		return;
	if ((startT + len) >= endT){
		CallBackFlg = true;
		if (LoopFlg){
			double flen = startT + len - endT;
			startT = TimeManage::getInstance()->GetCurCpuTime();
			endT = startT + timelen - flen;
		}
	}
	else{
		startT += len;
	}
}