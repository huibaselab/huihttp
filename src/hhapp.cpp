

#include <hlog.h>
#include <signal.h>

#include <event2/thread.h>

#include "hhapp.h"

Hhapp* g_app = nullptr;

Hhapp::Hhapp(HINT argc, const HCHAR* argv[])
    : HCApp(argc, argv){


}


Hhapp::~Hhapp() {

	HDELP(m_server);

}


HBOOL Hhapp::Run() {

    m_server->Run();

    return HTRUE;

}


void Hhapp::Stop () {

    m_server->Stop();

}



void Hhapp::init() { 
    
    // ensure multiple thread with libevent.
    evthread_use_pthreads();   

    // signal.
    HFAILED_THROW(setupSignal());

    // setup param.
    HFAILED_THROW(setupParam());

    // http server.
    HFAILED_THROW(initServer());    

    g_app = this;

}


void Hhapp::uninit() {

    Stop();

}


void Hhapp::signal_handler(int sig) {

    switch(sig) {
    case SIGTERM:
    case SIGHUP:
    case SIGQUIT:
    case SIGINT:

        SLOG_NS("signal break...");
        g_app->Stop();

        break;
    }

}


HRET Hhapp::setupSignal() {

    signal(SIGHUP, Hhapp::signal_handler);
    signal(SIGTERM, Hhapp::signal_handler);
    signal(SIGINT, Hhapp::signal_handler);
    signal(SIGQUIT, Hhapp::signal_handler);

    HRETURN_OK;
}


HRET Hhapp::setupParam() {

    HNOTOK_RETURN(m_param.Setup(m_conf));

    HRETURN_OK;

}



HRET Hhapp::initServer() throw (){

	m_server = new CHhServer(m_param);
	CHECK_NEWPOINT(m_server);

    HNOTOK_RETURN(m_server->Init());

    HRETURN_OK;
}

