/*******************************************************
 *
 * FileName: hhserver.h
 *
 * Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Mon Dec 16 16:57 2019
 *
 * Brief:
 *
 *
 *******************************************************/

#ifndef __HHSERVER_H__
#define __HHSERVER_H__

#include <huibase.h>
#include <hsocket.h>
#include <thread.h>

#include <nosql/hredis.h>

#include <event.h>
#include <evhttp.h>

#include "hhconfigparam.h"

using namespace HUIBASE;
using namespace HUIBASE::NOSQL;

class CHhServer {
public:
	typedef struct {
		CHhServer* server =  nullptr;
		struct event_base* base = nullptr;
		struct evhttp* httpd = nullptr;
		HINT thread_index {0};
	} WORK_ARG;

public:
	CHhServer(HhConfigParam& param);

	~CHhServer();

public:
	HRET Init();

	HRET Run ();

	void Stop ();

public:
	CRedis* GetMem (HCSTRR strName) ;

	HCSTRR GetBConfValue (HCSTRR key) const { return m_param.GetBConfValue(key);  }

	HBOOL IsBlackAddr (HCSTRR addr) const;

	HBOOL IsSupportTypes (HCSTRR type) const;

private:
	void uninitSocket ();	

private:
	static void httpserver_handler(struct evhttp_request* req, void* arg);

	static void* worker_handler(void* arg);

private:
	HRET initDataConn ();

	HRET initWorkData ();

	HRET initSocket ();	

	HRET initWorks();

private:
	HINT GetThreadIndex ();

	void SetThreadData (WORK_ARG* p);

private:
	static void get_client_info (struct evhttp_request* req, HSTRR strIp, HINT& nPort);	

	static HSTR get_http_req_type (struct evhttp_request* req);

private:
	HhConfigParam& m_param;

	HSYS_T m_fd{ 0 };

	HCTcpSocket m_tcp;

	WORK_ARG* m_thread_datas = nullptr;

	std::vector<CThread> m_threads;

	CThreadKey m_data_key;

};



#endif


