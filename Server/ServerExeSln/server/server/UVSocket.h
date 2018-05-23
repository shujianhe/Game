#pragma once
#include <iostream>
#include <string>
#include "uv.h"
#include <functional>
#define SENDDATAQUEUESIZE 50
typedef std::function<int(void*,int)> LambdaRecvBack;
namespace useruv{
	enum SendDataType
	{
		DEFAULT = 0,//无压缩 无加密
		ONLYCOMPRESS,//压缩
		ONLYADDKEY,//加密
		COMPRESSADNKEY//压缩 加密
	};
	struct SendData{//实际发送数据
		int len;//pdata 长度
		int time;//发送时候时间戳
		SendDataType Type;
		char *pdata;//lua提供数据
	};
	class SendDataQueueItem
	{
	private:
			int Maxlen;//sendata 内存大小
			char* senddata;
			int len;//senddata 数据大小
			bool Flg;//标志是否使用中
	public:
		SendDataQueueItem(){
			Maxlen = -1;
			senddata = NULL;
			len = -1;
			Flg = false;
		}
		bool setData(SendData* data){
			if (Flg)
				return false;
			int slen = sizeof(SendData)+data->len - sizeof(char*);
			if (slen > Maxlen){
				if (senddata){
					delete[] senddata;
				}
				senddata = new char[slen+1];
				Maxlen = slen + 1;
			}
			memcpy(senddata, data, sizeof(SendData)-sizeof(char*));
			memcpy(senddata + sizeof(SendData)-sizeof(char*), data->pdata, data->len);
			len = slen;
			Flg = true;
			return true;
		}
		SendData *getData(){
			if (Flg == false)
				return NULL;
			SendData *data = (SendData*)senddata;
			return data;
		}
		int getLen(){
			return len;
		}
		bool GetFlg(){
			return Flg;
		}
		void Clear(){
			Flg = false;
		}
	};
	class UVSocket
	{
		std::string ip;
		int port;
		int loop_Id;

		uv_tcp_t* m_tcp;
		uv_connect_t* m_connect;
		uv_async_t* m_async;

		char *RecvBufferData;//接受数据的临时空间
		int RecvBufferType;//内存申请方式  RecvBufferData本对象固有内存(RecvBufferType = 0) 否则表示RecvBufferData临时内存(RecvBufferType = (int)RecvBufferData)
		SendDataQueueItem SendQueue[SENDDATAQUEUESIZE];//缓存要发送的数据
		LambdaRecvBack LambdaBack;//回调 新连接 或者 Recv
	private:
		void deleteLoop();
	public:
		UVSocket(std::string ip,int port);
		UVSocket(uv_tcp_t *& tcp,int p_loop_id);
		~UVSocket();
		bool connect();
		bool Server();
		bool Send(char *data, int len);
		bool Send(SendData*& pdata);
		/*以下函数不能外部调用，只能本文件对应cpp调用*/
		//构造时链接处理函数
		void OnConnect(int status);
		//tcp接受数据回调函数
		void OnRecv(int len);
		//断开tcp函数
		void OnClose();
		//发送数据时候异步回调函数 主要是为了取发送的数据
		void onSendMsg();
		//接受内存 获取
		void onAlloc(size_t suggested_size, uv_buf_t* buf);
		//作为服务器部分 接收到新连接处理
		void onAccept(int status);
		void inline SetLambdaBack(LambdaRecvBack lambdaBack){
			LambdaBack = lambdaBack;
		}
	public:
		int State;
	};
}