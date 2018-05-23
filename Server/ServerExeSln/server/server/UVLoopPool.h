#pragma once
#include "uv.h"
#include <map>
namespace useruv{
	struct Loop{
		uv_loop_t* loop;
		int usercount;
		int errornumber;
		bool Flg;
		bool run();
	};
	class UVLoopPool
	{
		std::map<int, Loop*> looplist;
		static UVLoopPool* Pool;
		int SingleLoopMaxWorkNumber;
	private:
		UVLoopPool();
		~UVLoopPool();
	public:
		static UVLoopPool* getInstance(){
			Pool = Pool ? Pool : new UVLoopPool();
			return Pool;
		}
		static void Destroy(){
			if (Pool)
				delete Pool;
			Pool = NULL;
		}
		uv_loop_t* getuvloop(int& loop_Id);
		bool runLoop(int loop_Id);
		bool newLoop(int &loop_Id);
		void deleteLoop(int &loop_Id);
	};
}