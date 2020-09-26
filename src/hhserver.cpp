/*
 * @Author: Tom Hui
 * @Date: 2019-12-18 14:55:20
 * @Description:    
 */

#include <hlog.h>
#include "hhserver.h"
#include <haddr.h>
#include "apibase.h"
#include "dataconnmap.h"
#include <libconfig.h++>
#include "response.h"

CHhServer::CHhServer(HhConfigParam& param) 
	: m_param(param) {

    m_thread_datas = new WORK_ARG[m_param.GetWorkCount()];

    m_threads.resize(m_param.GetWorkCount());

}


CHhServer::~CHhServer (){

    uninitSocket();

    HDEL_ARR(m_thread_datas);

}


HRET CHhServer::Init() {

    HASSERT_THROW_MSG(HIS_OK(initWorkData()), "hhserver init thread special data failed", UN_INIT);

    HASSERT_THROW_MSG(HIS_OK(initDataConn()), "hhserver init data connection failed", UN_INIT);    

	HASSERT_THROW_MSG(HIS_OK(initSocket()), "hhserver init socket failed", UN_INIT);

    HASSERT_THROW_MSG(HIS_OK(initWorks()), "hhserver init work thread failed", UN_INIT);    

    HRETURN_OK;
}


HRET CHhServer::Run () {

    for (HSIZE i = 0; i < m_threads.size(); ++i) {

        m_threads[i].Join(nullptr);

    }

    SLOG_NS("hhserver stop running");
    HRETURN_OK;
}


void CHhServer::Stop () {

    for (HINT i = 0; i < m_param.GetWorkCount(); ++i) {

        WORK_ARG* p = m_thread_datas + i;
        struct event_base* base = p->base;
        SLOG_NORMAL("stop base[%p]", base);
        event_base_loopbreak(base);
		event_base_loopexit(base, nullptr);

    }

}


CRedis* CHhServer::GetMem (HCSTRR strName) {

    HINT index = GetThreadIndex();

    return DB_MAP::Instance()->GetMem(strName, index);

}


HBOOL CHhServer::IsBlackAddr (HCSTRR addr) const {

    return m_param.IsBlackAddr(addr);

}


HBOOL CHhServer::IsSupportTypes (HCSTRR type) const {

    return m_param.IsSupportTypes(type);

}


void CHhServer::uninitSocket () {

    m_tcp.Close();

}


#define HANDLE_RESPONSE(req,code,msg) do { \
        evhttp_send_reply(req, code, msg, buf); \
        evbuffer_free(buf); \
    } while(0)
    

void CHhServer::httpserver_handler(struct evhttp_request* req, void* arg) {

    CHhServer* server = (CHhServer*)arg;

    struct evbuffer* buf = evbuffer_new();
    HASSERT_THROW_MSG(buf != nullptr, "evbuffer_new failed", SRC_FAIL);

    HSTR strClientIp;
    HINT nClientPort;
    get_client_info(req, strClientIp, nClientPort);
    SLOG_NORMAL("thread[%d] handle get request from [%s:%d]", server->GetThreadIndex(), strClientIp.c_str(), nClientPort);

    IF_TRUE(server->IsBlackAddr(strClientIp)) {

        SLOG_WARNING("[%s] is in black list", strClientIp.c_str());
        HANDLE_RESPONSE(req, 400, "ERROR REQUEST");
        return;

    }

    HSTR cmdtype = get_http_req_type(req);
    IF_FALSE(server->IsSupportTypes(cmdtype)) {

        SLOG_WARNING("[%s] is not support", cmdtype.c_str());
        HANDLE_RESPONSE(req, 400, "ERROR REQUEST");
        return;

    }

    HSTR strCgi (evhttp_request_get_uri(req));
    strCgi = strCgi.substr(1);
    SLOG_NORMAL("type: [%s], cgi: [%s]", cmdtype.c_str(), strCgi.c_str());

    size_t post_len = EVBUFFER_LENGTH(req->input_buffer);
    SLOG_NORMAL("buffer length: [%d]", post_len);

    static constexpr unsigned int LEN = 102400;
    static char rbuf[LEN] = {0};
    memset(rbuf, 0, LEN);
    memcpy(rbuf, EVBUFFER_DATA(req->input_buffer), post_len);

    SLOG_NORMAL("post data: [%s]", rbuf);

    stringstream ss;
    HSTR res;
    CApiBase* papi = api_factory::Instance()->GetApi(strCgi);
    std::shared_ptr<CApiBase> autop(papi);
    if (papi == nullptr) {

        ss << "{ \"ret_code\":\"999\", \"ret_msg\":\"have no this cgi\"  }";

    } else {

        papi->SetServer(server);
        papi->SetClientIp(strClientIp);
        papi->SetClientPort(nClientPort);
        papi->SetRequestType(cmdtype);

        try {

            HFAILED_THROW(papi->Init(rbuf));
            HFAILED_THROW(papi->Work());
            Response res = papi->GetRes();

            IF_TRUE(res.Nonresponse()) {

                ss << "{\"ret_code\":\"0\", \"ret_msg\":\"ok\"}";

            } else {

                ss << "{\"ret_code\":\"0\", \"ret_msg\":\"ok\", \"result\": "
               << res.GetResponse() <<"}";

            }
            

        } catch (const HCException& ex) {

            ss << "{\"ret_code\":\"999\", \"ret_msg\":\""
               << ex.what() <<"\"}";

        } catch (...) {

            ss << "{ \"ret_code\":\"99999\", \"ret_msg\":\"unkown exception\"  }";

        }
    }

    res = ss.str();

    SLOG_NORMAL("cgi response: [%s]", res.c_str());
    evbuffer_add_printf(buf, "%s", res.c_str());

    //evhttp_send_reply(req, 200, "OK", buf);
    //evbuffer_free(buf);
    //auto client_fd = evhttp_request_get_connection(req)->fd;
    HANDLE_RESPONSE(req, 200, "OK");
    //close(client_fd);
    evhttp_connection_free (evhttp_request_get_connection(req));
    
}


void* CHhServer::worker_handler(void* arg) {

    WORK_ARG* work_arg = (WORK_ARG*)arg;
    work_arg->server->SetThreadData(work_arg);

	event_base_dispatch(work_arg->base);

    SLOG_NS("free httpd");
    evhttp_free(work_arg->httpd);
    
	return nullptr;

}


HRET CHhServer::initDataConn () {

    using namespace libconfig;
    Config cfg;

    try {

        cfg.readFile("../conf/conn.cfg");

    } catch (const FileIOException& fiex) {

        SLOG_ERROR("I/O error while read file. msg: [%s]", fiex.what());

        HRETURN(IO_ERR);

    } catch (const ParseException& pex ) {

        SLOG_ERROR("Parse config file failed at %s:%d--%s", pex.getFile(),
                  pex.getLine(), pex.getError());

        HRETURN(INVL_RES);

    }

    SLOG_NORMAL("init db connection count: [%d]", m_param.GetWorkCount());

    {

        const Setting& root = cfg.getRoot ();

        if (root.exists("dbs")) {

            const Setting& dbs = root["dbs"];

            for (HINT i = 0; i < dbs.getLength(); ++i) {

                HSTR str, strName;
                CONN_INFO ci;

                dbs[i].lookupValue("name", strName);
                dbs[i].lookupValue("dbname", ci.m_db);
                dbs[i].lookupValue("ip", ci.m_host);
                dbs[i].lookupValue("port", str);
                ci.m_port = HCStr::stoi(str);
                dbs[i].lookupValue("user", ci.m_user);
                dbs[i].lookupValue("pass", ci.m_pw);

                for (HINT j = 0; j < m_param.GetWorkCount(); ++j) {

                    CMyConnection* p = new CMyConnection ();
                    CHECK_NEWPOINT(p);

                    p->SetUtf8 ();
                    HASSERT_THROW_MSG(HIS_OK(p->Connect(ci)), "initDbConn connect to db failed", DB_DISCONN);

                    SLOG_NORMAL("[%d][%s][%s][%s][%s][%p]", j, strName.c_str(), ci.m_db.c_str(), ci.m_host.c_str(), str.c_str(), p);

                    DB_MAP::Instance()->AddConn(strName, j, p);

                }

            }

        }

    }


    {

        const Setting& root = cfg.getRoot ();

        if (root.exists("rds")) {

            const Setting& dbs = root["rds"];

            for (HINT i = 0; i < dbs.getLength(); ++i) {

                HSTR str;
                NoSqlConnectionInfo ci;

                dbs[i].lookupValue("name", ci.strName);
                dbs[i].lookupValue("ip", ci.strIp);
                dbs[i].lookupValue("port", str);
                ci.nPort = HCStr::stoi(str);

                for (HINT j = 0; j < m_param.GetWorkCount(); ++j) {

                    CRedis* p = new CRedis(ci);
                    CHECK_NEWPOINT(p);

                    HASSERT_THROW_MSG(HIS_OK(p->Init()), "initConn connect to redis failed", RDS_ERR);

                    SLOG_NORMAL("[%d][%s][%s][%d]", j, ci.strName.c_str(), ci.strIp.c_str(), ci.nPort);

                    DB_MAP::Instance()->AddMem(ci.strName, j, p);

                }
            }
        }

    }


    /*{

        const Setting& root = cfg.getRoot ();

        if (root.exists("cls")){

            const Setting& dbs = root["cls"];

            for (HINT i = 0; i < dbs.getLength(); ++i) {

                HSTR strName, strIp, strPort, strTimeout, strPassword;
                HUINT nPort = 0, nTimeout = 0;

                dbs[i].lookupValue("name", strName);
                dbs[i].lookupValue("ip", strIp);
                dbs[i].lookupValue("port", strPort);
                dbs[i].lookupValue("timeout", strTimeout);

                if (dbs[i].exists("password")) {
                    dbs[i].lookupValue("password", strPassword);
                }

                nPort = HCStr::stoi(strPort);
                nTimeout = HCStr::stoi(strTimeout);

                SLOG_NORMAL("[%d][%s][%s]", i, strName.c_str(), strIp.c_str(), strPort.c_str());

                CMidClient* p = new CMidClient(strIp, nPort, nTimeout);
                CHECK_NEWPOINT(p);
                p->SetPassword(strPassword);

                m_clients.insert(CLIENT_MAP::value_type(strName, p));

            }

        }

    }*/


    HRETURN_OK;

}


HRET CHhServer::initWorkData () {

    HNOTOK_RETURN(m_data_key.Create(nullptr));

    HRETURN_OK;
}


HRET CHhServer::initSocket() {
    
    SLOG_NS("init tcp");
	HNOTOK_RETURN(m_tcp.Init());

    SLOG_NS("set reuseaddr");
	HNOTOK_RETURN(m_tcp.SetReuseAddr());

	HCIp4Addr addr(m_param.GetIP(), m_param.GetPort());

    SLOG_NS("bind socket");
	HNOTOK_RETURN(m_tcp.Bind(addr));

    SLOG_NS("set nonblocked");
	HNOTOK_RETURN(m_tcp.SetNonblocked());

    SLOG_NS("listen socket");
	HNOTOK_RETURN(m_tcp.Listen(m_param.GetListenLen()));

    SLOG_NORMAL("socket bind at[%s:%d] success", m_param.GetIP().c_str(), m_param.GetPort());
	HRETURN_OK;
}


HRET CHhServer::initWorks() {

    SLOG_NORMAL("hhserver will setup %d work thread", m_param.GetWorkCount());
	for (HINT i = 0; i < m_param.GetWorkCount(); ++i) {

		struct event_base* base = event_init();
		CHECK_NEWPOINT(base);

		struct evhttp* httpd = evhttp_new(base);
		CHECK_NEWPOINT(httpd);

		auto cb = evhttp_accept_socket(httpd, m_tcp.GetSocket());
		HASSERT_THROW_MSG(cb == 0, "libevent accept socket failed", SRC_FAIL);

        WORK_ARG* parg = m_thread_datas + i;
        parg->server = this;
        parg->base = base;
        parg->httpd = httpd;
        parg->thread_index = i;

		evhttp_set_gencb(httpd, CHhServer::httpserver_handler, this);

		m_threads[i].Create(CHhServer::worker_handler, parg);

	}

    HRETURN_OK;
}


HINT CHhServer::GetThreadIndex () {

    WORK_ARG* work_arg = (WORK_ARG*)m_data_key.Get();
    return work_arg->thread_index;

}


void CHhServer::SetThreadData (WORK_ARG* p) {

    m_data_key.Set(p);

}



void CHhServer::get_client_info (struct evhttp_request* req, HSTRR strIp, HINT& nPort) {

    HCHAR* szClientIp = nullptr;
    u_short nClientPort = 0;
    evhttp_connection_get_peer(evhttp_request_get_connection(req), &szClientIp, &nClientPort);
    evhttp_connection_set_flags(evhttp_request_get_connection(req), EVHTTP_CON_REUSE_CONNECTED_ADDR);
    evhttp_connection_free_on_completion(evhttp_request_get_connection(req));

    strIp = szClientIp;
    nPort = nClientPort;

}


HSTR CHhServer::get_http_req_type (struct evhttp_request* req) {

    HSTR cmdtype;
    switch (evhttp_request_get_command(req))
    {
        case EVHTTP_REQ_GET:    cmdtype = "GET";    break;
        case EVHTTP_REQ_POST:   cmdtype = "POST";   break;
        case EVHTTP_REQ_HEAD:   cmdtype = "HEAD";   break;
        case EVHTTP_REQ_PUT:    cmdtype = "PUT";    break;
        case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
        case EVHTTP_REQ_OPTIONS:cmdtype = "OPTIONS";break;
        case EVHTTP_REQ_TRACE:  cmdtype = "TRACE";  break;
        case EVHTTP_REQ_CONNECT:cmdtype = "CONNECT";break;
        case EVHTTP_REQ_PATCH:  cmdtype = "PATCH";  break;
        default: cmdtype = "unknown"; break;
    }

    return cmdtype;
}

