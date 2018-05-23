#pragma once
#include "UVSocket.h"
#include <map>
#include <list>
#include <ctime>
#include <cassert>
namespace useruv{
#define SOCKETHANDLE int
	struct UVSocketItem{
		std::string ip;
		int port;
		SOCKETHANDLE hand;
	};
	class UVSocketManage
	{
		static UVSocketManage* UVSM;
		std::map<int, UVSocketItem*> socketmap;
		LambdaRecvBack* RecvBack;
		LambdaRecvBack* acceptBack;
	private:
		UVSocketManage();
		~UVSocketManage();
		inline UVSocket *Hand2UVSocket(SOCKETHANDLE hand){
			auto iter = socketmap.find(hand);
			if (iter == socketmap.end())
				return NULL;
			return (UVSocket*)(iter->second->hand);
		}
		inline SOCKETHANDLE FindHand(std::string ip, int port){
			for each (auto var in socketmap)
			{
				UVSocketItem* item = var.second;
				if (ip == item->ip && port == item->port){
					return var.first;
				}
			}
			return 0;
		}
		int OnAccept(void* buf, int len);
		int OnRecv(void* buf, int len);
	public:
		static UVSocketManage* getInstance(){
			UVSM = UVSM ? UVSM : new UVSocketManage();
			return UVSM;
		}
		static void Destroy(){
			if (UVSM)
				delete UVSM;
			UVSM = NULL;
		}
		inline void InitLambdaFunction(LambdaRecvBack* RecvBack, LambdaRecvBack* acceptBack){
			this -> RecvBack = RecvBack;
			this->acceptBack = acceptBack;
		}
		inline SOCKETHANDLE Server(int port){
			return Server("0,0,0,0", port);
		}
		SOCKETHANDLE connect(std::string ip, int port);
		SOCKETHANDLE Server(std::string ip, int port);
		bool Send(SOCKETHANDLE hand, char *data, int len);
		int Find(std::string ip, int port);
		void close(SOCKETHANDLE hand);
		//int connect(std::string ip, int port, LambdaRecvBack back);
	};
	class UVRecvDataQueue{
		static UVRecvDataQueue* UVRDQ;
		std::list<char*> qdlist;
	private:
		UVRecvDataQueue(){
		}
		~UVRecvDataQueue(){
			for each (auto item in qdlist)
			{
				delete[] item;
				item = NULL;
			}
			qdlist.clear();
		}
	public:
		static UVRecvDataQueue *getInstance(){
			UVRDQ = UVRDQ ? UVRDQ : new UVRecvDataQueue();
			return UVRDQ;
		}
		static void Destroy(){
			if (UVRDQ)
				delete UVRDQ;
			UVRDQ = NULL;
		}
		inline void push(SendData *data,int hand){
			//hand len sendtime recvtime data
			int size = sizeof(SendData)+data->len;// +sizeof(int);
			int offset = 0;
			char *pdata = new char[size+1];
			memcpy(&pdata[offset], &hand, sizeof(hand));
			assert((*(int*)pdata) == hand);
			offset += sizeof(hand);
			memcpy(&pdata[offset], data, sizeof(SendData)-sizeof(char*));
			offset += sizeof(SendData)-sizeof(char*);
			char *sdata = (char*)data;
			memcpy(&pdata[offset], &sdata[sizeof(SendData)-sizeof(char*)], data->len);
			offset = sizeof(int)* 3;//通过占用 Type 记录recvtime
			int recvtime = (int)time(NULL);
			memcpy(&pdata[offset], &recvtime, sizeof(int));
			pdata[size] = '\0';
			qdlist.push_back(pdata);
		}
		//hand len sendtime recvtime data
		inline bool pop(std::function < void(int, int, int, int, char*)> back){
			if (qdlist.size() == 0)
				return false;
			char *pdata = *qdlist.begin();
			int hand = *(int*)pdata;
			int len = *(int*)&pdata[4];
			int sendtime = *(int*)&pdata[8];
			int recvtime = *(int*)&pdata[12];
			char* data = (char*)&pdata[4 * sizeof(int)];
			back(hand, len, sendtime, recvtime, data);
			qdlist.pop_front();
			delete[] pdata;
			pdata = NULL;
			return true;
		}
	};
}