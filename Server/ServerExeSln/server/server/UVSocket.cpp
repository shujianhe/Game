#include "stdafx.h"
#include "UVSocket.h"
#include "UVLoopPool.h"
#include "LoadIni.h"
#include <mutex>
#include <sstream>
#define SELF UVSocket
#define RECVMAXBUFFERSIZE  1024*100
#define SERVERMAXCLIENTCONNECT 1024*5
using namespace useruv;
static int ListenCount = -1;
static int sg_client[SERVERMAXCLIENTCONNECT] = { 0 };
static void on_connect(uv_connect_t* req, int status);
static void tcp_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
static void tcp_readclient(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf);
static void tcp_close(uv_handle_t* handle);
static void async_cb(uv_async_t *async);
static void write_cb(uv_write_t* req, int status);
static void on_new_connection(uv_stream_t* server, int status);

SELF::SELF(std::string ip, int port) :ip(ip), port(port), m_async(NULL), m_connect(NULL), m_tcp(NULL){
	UVLoopPool::getInstance()->newLoop(loop_Id);
	RecvBufferData = NULL;
	RecvBufferType = 0;
}
SELF::SELF(uv_tcp_t *& tcp, int p_loop_id) : m_connect(NULL), ip(""), port(0)//暂时不知道怎么获取
{
	this->loop_Id = p_loop_id;
	RecvBufferData = NULL;
	RecvBufferType = 0;
	m_tcp = tcp;
	m_tcp->data = this;
	m_async = new uv_async_t;
	m_async->data = this;
	RecvBufferType = 0;
	RecvBufferData = new char[RECVMAXBUFFERSIZE];
	uv_loop_t *loop = UVLoopPool::getInstance()->getuvloop(loop_Id);
	uv_async_init(loop, m_async, async_cb);
	uv_read_start((uv_stream_t *)m_tcp, tcp_alloc, tcp_readclient);
}
SELF::~SELF()
{
	if (m_tcp){
		m_tcp->data = NULL;
		uv_close((uv_handle_t*)m_tcp, tcp_close);
		m_tcp = NULL;
		Sleep(5);
		this->OnClose();
	}
	else
		deleteLoop();
}
void SELF::deleteLoop(){
	std::mutex t;
	t.lock();
	UVLoopPool::getInstance()->deleteLoop(loop_Id);
	t.unlock();
}
bool SELF::connect(){
	if (m_tcp)
		return false;
	if (ip == "")
		return false;
	if (port == 0)
		return false;

	m_tcp = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	m_connect = new uv_connect_t;
	uv_loop_t * t_loop = UVLoopPool::getInstance()->getuvloop(loop_Id);
	if (t_loop == NULL)
		return false;
	struct sockaddr_in dest;
	int r = uv_ip4_addr(ip.c_str(), port, &dest);
	if (r != 0){
		deleteLoop();
		free(m_tcp);
		m_tcp = NULL;
		delete m_connect;
		m_connect = NULL;
		std::cout << "初始化UVSocket失败之uv_ip4_addr Error:" << uv_strerror(r);
		State = -1;
		return false;
	}
	m_connect->data = this;
	m_tcp->data = this;
	uv_tcp_init(t_loop, m_tcp);
	r = uv_tcp_connect(m_connect, m_tcp, (sockaddr*)&dest, on_connect);
	if (r != 0){
		deleteLoop();
		free(m_tcp);
		m_tcp = NULL;
		delete m_connect;
		m_connect = NULL;
		std::cout << "初始化UVSocket失败之uv_thread_create Error:" << uv_strerror(r);
		State = -1;
		return false;
	}
	m_async = new uv_async_t;
	m_async->data = this;
	uv_async_init(t_loop, m_async, async_cb);
	UVLoopPool::getInstance()->runLoop(loop_Id);
	int count = 0;
	while (true){
		switch (State)
		{
		case 1:
			return true;
		case -1:
			return false;
		default:
			break;
		}
		Sleep(15);
		count = count + 1;
		if (count > 50)
			State = -1;
	}
	return false;
}
bool SELF::Server(){
	if (m_tcp)
		return false;
	if (ip == "")
		return false;
	if (port == 0)
		return false;
	if (ListenCount > 0)
		return false;
	memset(sg_client, 0, sizeof(sg_client));
#define DEFAULT_LISTEN 99900
	int temp_ListenCount = atoi(INI("listenCount").c_str());
	if (temp_ListenCount < 1 || temp_ListenCount >= SERVERMAXCLIENTCONNECT){
		temp_ListenCount = DEFAULT_LISTEN;
		std::stringstream ss;
		ss << temp_ListenCount;
		std::string s;
		ss >> s;
		INI("listenCount") = s;
	}
#undef DEFAULT_LISTEN
	ListenCount = temp_ListenCount;
	m_tcp = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	int r = uv_tcp_init(UVLoopPool::getInstance()->getuvloop(loop_Id), m_tcp);
	sockaddr_in addr;
	uv_ip4_addr(ip.c_str(), this->port, &addr);
	r = uv_tcp_bind(m_tcp, (const struct sockaddr*)&addr, 0);
	m_tcp->data = this;
	r = uv_listen((uv_stream_s*)m_tcp, ListenCount, on_new_connection);
	if (r){
		fprintf(stderr, "Listen error %s\n", uv_strerror(r));
		return false;
	}
	else{
		UVLoopPool::getInstance()->runLoop(loop_Id);
		return true;
	}
}
/*以下函数不能外部调用，只能本文件对应cpp调用*/
//构造时链接处理函数
void SELF::OnConnect(int status){
	if (status != 0){
		printf("error :%s\n", uv_strerror(status));
		free(m_tcp);
		m_tcp = NULL;
		goto ErrorDel;
	}

	int r = uv_read_start((uv_stream_t *)(this->m_tcp), tcp_alloc, tcp_readclient);
	if (r != 0) {
		printf("uv_read_start:%s\n", uv_strerror(r));
		goto ErrorDel;
	}
	RecvBufferType = 0;
	RecvBufferData = new char[RECVMAXBUFFERSIZE];
	State = 1;
	return;
ErrorDel:{
	deleteLoop();
	delete m_connect;
	m_connect = NULL;
	this->State = -1;
	return;
}
}
void SELF::onAlloc(size_t suggested_size, uv_buf_t* buf){
	const int t_intsize = sizeof(int);
	if (suggested_size > (RECVMAXBUFFERSIZE - t_intsize)){
		/*RecvBufferType = (int)RecvBufferData;
		RecvBufferData = new char[suggested_size + t_intsize];*/
		suggested_size = RECVMAXBUFFERSIZE - t_intsize;
	}
	int t_ithis = (int)this;
	memcpy(RecvBufferData, &t_ithis, t_intsize);
	buf->base = &RecvBufferData[t_intsize];
	buf->len = suggested_size;
}
//tcp接受数据回调函数
void SELF::OnRecv(int len){
	if (len < 0)
	{
		LambdaBack(this, len);
		return;
	}
	if (len > 0){
		RecvBufferData[sizeof(int) + len] = '\0';
	}
	LambdaBack(RecvBufferData, len);
	if (RecvBufferType != 0){
		delete[] RecvBufferData;
		RecvBufferData = (char*)RecvBufferType;
		RecvBufferType = 0;
	}
}
//断开tcp函数
void SELF::OnClose(){
	deleteLoop();	
	if (m_connect)
	{
		delete m_connect;
		m_connect = NULL;
	}
	if (m_async){
		delete m_async;
		m_async = NULL;
	}
	std::mutex t_mutex;
	t_mutex.lock();
	if (RecvBufferData != NULL){
		delete RecvBufferData;
		RecvBufferData = NULL;
	}
	if (RecvBufferType != 0){
		RecvBufferData = (char*)RecvBufferType;
		delete RecvBufferData;
		RecvBufferData = NULL;
		RecvBufferType = 0;
	}
	t_mutex.unlock();
	State = 0;
	if (m_tcp){
		free(m_tcp);
		m_tcp = NULL;
	}
}
bool SELF::Send(char *data, int len){
	SendData *sdata = new SendData;
	sdata->time = (int)time(NULL);
	sdata->Type = SendDataType::DEFAULT;
	sdata->len = len;
	sdata->pdata = data;
	return Send(sdata);
}
bool SELF::Send(SendData *&pdata){
	int count = 0;
	std::mutex t_mutex;
	int Id = -1;
	FindSendQueueBuffer:
	t_mutex.lock();
	for (int i = 0; i < SENDDATAQUEUESIZE; i++){
		if (SendQueue[i].GetFlg() == false){
			Id = i;
			break;
		}
	}
	t_mutex.unlock();
	if (Id == -1)
	{
		count = count + 1;
		if (count < 50){
			Sleep(10);
			goto FindSendQueueBuffer;
		}

	}
	if (Id > -1){
		t_mutex.lock();
		SendQueue[Id].setData(pdata);
		t_mutex.unlock();
		uv_async_send(this->m_async);
	}
	delete pdata;
	pdata = NULL;
	return Id > -1;
}
//发送数据时候异步回调函数 主要是为了取发送的数据
void SELF::onSendMsg(){
	int Id = -1;
	std::mutex t_mutex;
	t_mutex.lock();
	for (int i = 0; i < SENDDATAQUEUESIZE; i++){
		if (SendQueue[i].GetFlg() == true){
			Id = i;
			break;
		}
	}
	if (Id == -1){
		t_mutex.unlock();
		return;
	}
	uv_write_t *write_req = new uv_write_t;
	uv_buf_t *bufs = new uv_buf_t;
	int size = SendQueue[Id].getLen();
	SendQueue[Id].getData();
	bufs->base = new char[size + 1];
	memcpy(bufs->base, SendQueue[Id].getData(), size);
	t_mutex.unlock();
	bufs->base[size] = '\0';
	bufs->len = size;
	write_req->data = bufs;
	//发送
	int r = uv_write(write_req, (uv_stream_t*) this->m_tcp, bufs, 1, write_cb);
	if (r < 0){
		write_cb(write_req, r);
	}
	return;
}
void SELF::onAccept(int status){
	if (status < 0){
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		return;
	}
	int i = 0;
	for (i = 0; i < ListenCount; i++){
		if (sg_client[i] == 0)break;
	}
	if (i == ListenCount){
		printf("连接队列已满");
		return;
	}
	uv_tcp_t * client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	int loop_Id = 0;
	UVLoopPool::getInstance()->newLoop(loop_Id);
	uv_tcp_init(UVLoopPool::getInstance()->getuvloop(loop_Id),client);
	if (uv_accept((uv_stream_t*)m_tcp, (uv_stream_t*)client) == 0){
		int Id = LambdaBack((void*)client, loop_Id);
		sg_client[i] = Id;
	}
	else{
		uv_close((uv_handle_t*)client, NULL);
	}
}
//uv 回调系列静态函数
static void on_connect(uv_connect_t* req, int status){
	UVSocket *uvs = (UVSocket*)(req->data);
	uvs->OnConnect(status);
}
static void tcp_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf){
	SELF *uvs = (SELF*)(handle->data);
	uvs->onAlloc(suggested_size, buf);
}
static void tcp_readclient(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf){
	SELF *uvs = (SELF*)(tcp->data);
	uvs->OnRecv(nread);
}
static void tcp_close(uv_handle_t* handle){
	SELF *uvs = (SELF*)handle->data;
	if (uvs){
		uvs->OnClose();
	}
	else
	{
		free(handle);
		handle = NULL;
	}
}
static void async_cb(uv_async_t *async){
	SELF *handle = (SELF*)async->data;
	handle->onSendMsg();
}
static void write_cb(uv_write_t* req, int status){
	if (status){
		printf("send write_cb Error:%s\n", uv_strerror(status));
	}
	uv_buf_t* buf = (uv_buf_t*)req->data;
	delete[] buf->base;
	buf->base = NULL;
	delete buf;
	buf = NULL;
	delete req;
	req = NULL;
}
static void on_new_connection(uv_stream_t* server, int status){
	((SELF*)server->data)->onAccept(status);
}
#undef SELF
