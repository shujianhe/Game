#include "stdafx.h"
#include "UVLoopPool.h"
#include "LoadIni.h"
#include <mutex>
#include <thread>
#include "Tools.h"
#define SELF UVLoopPool
using namespace useruv;
bool Loop::run(){
	if (Flg == true)
		return errornumber == 0;
	int r = -1;
	Flg = true;
	std::thread thr([this,&r](){
		uv_run(loop, UV_RUN_DEFAULT);
	});
	thr.detach();
	return true;
}
SELF *SELF::Pool = NULL;
SELF::SELF()
{
	looplist.clear();
	SingleLoopMaxWorkNumber = 200;
	int ininumber = atoi(INI("SingleLoopMaxWorkCount").c_str());
	if (ininumber > 0 && ininumber < 255){
		SingleLoopMaxWorkNumber = ininumber;
	}
}

SELF::~SELF()
{
	LogDebug("回收所有uv流水线");
	for each (auto var in looplist)
	{
		if (var.second && var.second->loop){
			uv_loop_close(var.second->loop);
			Sleep(10);
			var.second->loop = NULL;
			delete var.second;
			var.second = NULL;
		}
	}
	looplist.clear();
}
uv_loop_t* SELF::getuvloop(int& loop_Id){
	if (loop_Id >= 0){
		std::map<int, Loop*>::iterator iter = looplist.find(loop_Id);
		
		if (iter == looplist.end())
			return NULL;
		return iter->second->loop;
	}
	return NULL;
}
bool SELF::runLoop(int loop_Id){
	std::map<int, Loop*>::iterator iter = looplist.find(loop_Id);
	if (iter == looplist.end())
		return false;
	return iter->second->run();
}
bool SELF::newLoop(int &loop_Id){
	if (loop_Id > 0)
		return false;
	int Id = -1;
	for each (auto var in looplist)
	{
		Loop *loop = var.second;
		if (loop->usercount < SingleLoopMaxWorkNumber){
			Id = var.first;
			break;
		}
	}
	if (Id == -1){
		Loop * loop = new Loop;
		loop->usercount = 0;
		loop->loop = uv_loop_new();
		loop->Flg = false;
		loop->errornumber = 0;
		Id = looplist.size();
		looplist[Id] = loop;
	}
	std::mutex t_mutex;
	t_mutex.lock();
	looplist[Id]->usercount = looplist[Id]->usercount + 1;
	t_mutex.unlock();
	loop_Id = Id;
	return true;
}
void SELF::deleteLoop(int &loop_Id){
	if (loop_Id > 0){
		std::map<int, Loop*>::iterator iter = looplist.find(loop_Id);
		if (iter == looplist.end()){
			iter->second->usercount = iter->second->usercount - 1;
		}
	}
	loop_Id = -1;
}
#undef SELF