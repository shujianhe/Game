#include "stdafx.h"
#include "UVSocketManage.h"
#include "Tools.h"
#include "UVLoopPool.h"
#include <assert.h>
using namespace useruv;
#define SELF UVSocketManage
UVRecvDataQueue* UVRecvDataQueue::UVRDQ = NULL;
SELF *SELF::UVSM = NULL;
static std::map<int, int*> RecvHandBuffer;
SELF::SELF()
{
}
SELF::~SELF()
{
	LogDebug("回收socket资源:%s", "断开所有socket");
	for each (auto var in socketmap)
	{
		UVSocket * hand = (UVSocket*)(var.second->hand);
		delete hand;
		hand = NULL;
		var.second->hand = 0;
		delete var.second;
		var.second = NULL;
	}
	char *p = "sdfsdf";
	char *pp = new char[5];
	LogDebug("回收socket资源:%s", "回收分包 并包的临时堆空间");
	for each (auto var in RecvHandBuffer)
	{
		if (var.second != 0){
			if (var.second <= (int*)&p){//不是栈空间
				if (var.second < (int*)&pp){//是堆空间
					delete[] var.second;
					var.second = NULL;
				}
			}
		}
	}
	delete pp;
	RecvHandBuffer.clear();
	socketmap.clear();
	Sleep(40);
	UVLoopPool::Destroy();
}
SOCKETHANDLE SELF::connect(std::string ip, int port){
	SOCKETHANDLE hand = FindHand(ip, port);
	if (hand != 0){
		assert(false);
		return hand;
	}
	UVSocket* uvhand = new UVSocket(ip, port);
	if (false == uvhand->connect()){
		delete uvhand;
		uvhand = NULL;
		return 0;
	}
	auto recvback = [&](void* buf, int len)->int{
		return this->OnRecv(buf, len);
	};
	uvhand->SetLambdaBack(recvback);
	hand = (int)uvhand;
	struct UVSocketItem *Item = new struct UVSocketItem;
	Item->hand = hand;
	Item->ip = ip;
	Item->port = port;
	socketmap[Item->hand] = Item;
	return hand;
}
SOCKETHANDLE SELF::Server(std::string ip, int port){
	SOCKETHANDLE hand = FindHand(ip, port);
	if (hand != 0){
		assert(false);
		return hand;
	}
	UVSocket* uvhand = new UVSocket(ip, port);
	if (false == uvhand->Server()){
		delete uvhand;
		uvhand = NULL;
		return 0;
	}
	auto recvback = [&](void* buf, int len)->int{
		return this->OnAccept(buf, len);
	};
	uvhand->SetLambdaBack(recvback);
	hand = (int)uvhand;
	struct UVSocketItem *Item = new struct UVSocketItem;
	Item->hand = hand;
	Item->ip = ip;
	Item->port = port;
	socketmap[Item->hand] = Item;
	return hand;
}
bool SELF::Send(SOCKETHANDLE hand, char *data, int len){
	UVSocket *uvs = this->Hand2UVSocket(hand);
	if (uvs == NULL)
		return false;
	return uvs->Send(data, len);
}
int SELF::Find(std::string ip, int port){
	return this->FindHand(ip, port);
}
void SELF::close(SOCKETHANDLE hand){
	UVSocket *uvs = Hand2UVSocket(hand);
	if (uvs){
		delete uvs;
		uvs = NULL;
		delete socketmap[hand];
		socketmap.erase(socketmap.find(hand));
	}

	char *p = "sdfsdf";
	char *pp = new char[5];
	LogDebug("回收socket资源:%s", "回收分包 并包的临时堆空间");
	auto iter = RecvHandBuffer.find(hand);
	if (iter != RecvHandBuffer.end()){
		if (iter->second != NULL){
			if (iter->second <= (int*)&p){//不是栈空间
				if (iter->second < (int*)&pp){//是堆空间
					delete[] iter->second;
					iter->second = NULL;
				}
			}
		}
		RecvHandBuffer.erase(iter);
	}
	delete pp;
}
int SELF::OnAccept(void* buf, int len){
	uv_tcp_t* tcp = (uv_tcp_t*)buf;
	UVSocket* uvs = new UVSocket(tcp,len);
	uvs->SetLambdaBack([&](void* buf, int len)->int{
		return this->OnRecv(buf, len);
	});
	struct UVSocketItem *Item = new struct UVSocketItem;
	Item->hand = (int)uvs;
	Item->ip = "";
	Item->port = 0;
	socketmap[Item->hand] = Item;
	return Item->hand;
}
int SELF::OnRecv(void* buf, int len){
	if (len < 0){
		LogDebug("Recv 有可能是断开连接错误 :%s", uv_strerror(len));
		int hand = (int)buf;
		close(hand);
		return 0;
	}
	int hand = *(int*)buf;
	int offset = 0;
	char *cbuf = NULL;
	if (RecvHandBuffer[hand]){
		char *cbuf = (char *)RecvHandBuffer[hand];
		SendData *send = (SendData*)&cbuf[4];
		offset = *(int*)cbuf;
		if (send->len > (len + offset - sizeof(SendData))){
			char* ttemp =(char*)buf+4;
			for (int i = 0; i < len; i++)
				cbuf[offset + i] = ttemp[i];
			offset += len;
			memcpy(cbuf, &offset, sizeof(int));
			return 0;
		}
		else{
			int plen = sizeof(SendData)+sizeof(int)+send->len - offset;
			char* ttemp = (char*)buf + 4;
			for (int i = 0; i < plen; i++)
			{
				cbuf[offset + i] = ttemp[i];
			}
			int end = plen + offset - 1;
			cbuf[end] = '\0';
			UVRecvDataQueue::getInstance()->push(send, hand);
			delete[] cbuf;
			cbuf = NULL;
			RecvHandBuffer[hand] = NULL;
			if (plen >= len)
				return 0;
			else if (plen < len)
			{
				offset = plen+1;
				cbuf = (char*)(buf)+offset;
			}
		}
	}
	else{
		offset =0;
		cbuf = (char*)(buf) + 4;
	}
	int constSize = sizeof(SendData)-sizeof(char*);
	while (true){
		//cbuf = (char*)(buf)+offset;
		SendData *send = (SendData *)cbuf;
		if ((constSize + send->len) > len){
			char *mapbuf = new char[sizeof(SendData)+sizeof(int)+send->len];
			int newoffset = len - offset+sizeof(4);
			memcpy(mapbuf, &newoffset, 4);
			memcpy(&mapbuf[4], cbuf,len - offset);
			RecvHandBuffer[hand] = (int*)mapbuf;
			break;
		}
		else{
			UVRecvDataQueue::getInstance()->push(send, hand);
			offset = offset + constSize + send->len;
			if ((offset + 1) == len)//减去之前记录socket 地址int 加上 len 后面的\0
				break;
			else if (offset > len)
				return 0;
		}
	}
	return 0;
}