/*
 * @Author: Tom Hui
 * @Date: 2019-12-19 18:13:45
 * @Description: 
 */

#ifndef __HHCONFIGPARAM_H__
#define __HHCONFIGPARAM_H__

#include <huibase.h>
#include <hconf.h>

using namespace HUIBASE;


class HhConfigParam {
public:
    HhConfigParam () { }

    ~ HhConfigParam() { }

public:
    HRET Setup (const HCMapConf& conf);

    HCSTRR GetServerName () const { return m_strServerName; }

    HCSTRR GetIP () const { return m_strIp; }

    HINT GetPort () const { return m_nPort; }

    HINT GetListenLen () const { return m_nListenLen; }

    HINT GetWorkCount () const { return m_nWorkCount; }

    HCSTRR GetBConfValue (HCSTRR key) const;

    HBOOL IsBlackAddr (HCSTRR strAddr) const;

    HBOOL IsSupportTypes (HCSTRR type) const;

private:
    HRET setupBackIp (HCSTRR ips); 

    HRET setupSupportTypes (HCSTRR types);  

private:
    HSTR m_strServerName;

    HSTR m_strIp;

    HINT m_nPort {0};

    HINT m_nListenLen {0};

    HINT m_nWorkCount {16};

    HVSTRS m_back_ips;

    HVSTRS m_support_types;

    HCMapConf m_conf;

};



#endif 

