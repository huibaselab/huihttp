/*
 * @Author: Tom Hui
 * @Date: 2019-12-18 14:55:20
 * @Description: 
 */


#ifndef __HHAPP_H__
#define __HHAPP_H__

#include <huibase.h>
#include <happ.h>
#include "hhserver.h"
#include "hhconfigparam.h"

using namespace HUIBASE;

class Hhapp : public HCApp {
 public:
    Hhapp(HINT argc, const HCHAR* argv[]);

    ~Hhapp();

    HBOOL Run ();

    void Stop ();

 private:
    virtual void init();

    void uninit();

    HRET setupSignal();

    HRET setupParam ();

    HRET initServer () throw ();

private:
    static void signal_handler(int sig);   

 private:
    HhConfigParam m_param;

    CHhServer* m_server = nullptr;

};


#endif // __HHAPP_H__
