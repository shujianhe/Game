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
		DEFAULT = 0,//��ѹ�� �޼���
		ONLYCOMPRESS,//ѹ��
		ONLYADDKEY,//����
		COMPRESSADNKEY//ѹ�� ����
	};
	struct SendData{//ʵ�ʷ�������
		int len;//pdata ����
		int time;//����ʱ��ʱ���
		SendDataType Type;
		char *pdata;//lua�ṩ����
	};
	class SendDataQueueItem
	{
	private:
			int Maxlen;//sendata �ڴ��С
			char* senddata;
			int len;//senddata ���ݴ�С
			bool Flg;//��־�Ƿ�ʹ����
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

		char *RecvBufferData;//�������ݵ���ʱ�ռ�
		int RecvBufferType;//�ڴ����뷽ʽ  RecvBufferData����������ڴ�(RecvBufferType = 0) �����ʾRecvBufferData��ʱ�ڴ�(RecvBufferType = (int)RecvBufferData)
		SendDataQueueItem SendQueue[SENDDATAQUEUESIZE];//����Ҫ���͵�����
		LambdaRecvBack LambdaBack;//�ص� ������ ���� Recv
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
		/*���º��������ⲿ���ã�ֻ�ܱ��ļ���Ӧcpp����*/
		//����ʱ���Ӵ�����
		void OnConnect(int status);
		//tcp�������ݻص�����
		void OnRecv(int len);
		//�Ͽ�tcp����
		void OnClose();
		//��������ʱ���첽�ص����� ��Ҫ��Ϊ��ȡ���͵�����
		void onSendMsg();
		//�����ڴ� ��ȡ
		void onAlloc(size_t suggested_size, uv_buf_t* buf);
		//��Ϊ���������� ���յ������Ӵ���
		void onAccept(int status);
		void inline SetLambdaBack(LambdaRecvBack lambdaBack){
			LambdaBack = lambdaBack;
		}
	public:
		int State;
	};
}