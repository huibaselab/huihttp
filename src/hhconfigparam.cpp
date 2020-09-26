/*
 * @Author: Tom Hui
 * @Date: 2019-12-19 18:28:33
 * @Description: 
 */

#include <hlog.h>
#include "hhconfigparam.h"

HRET HhConfigParam::Setup (const HCMapConf& conf) {

    m_strServerName = conf.GetValue("serverName", "hhserver");

    m_strIp = conf.GetValue("ip", "127.0.0.1");

    m_nPort = conf.GetIntValue("port", 28000);

    m_nListenLen = conf.GetIntValue("listenLen", 32);

	m_nWorkCount = conf.GetIntValue("workcount", 16);

    HSTR strBConf = conf.GetValue("business_conf", "bconf");

    SLOG_NORMAL("ip[%s] port[%d] workCount[%d]", m_strIp.c_str(), m_nPort, m_nWorkCount);

    m_conf.LoadConfFile(strBConf);

    IF_TRUE(conf.IsHere("back_ips")) {
        HCSTRR strBackIps = conf.GetValue("back_ips");
        setupBackIp(strBackIps);
        SLOG_NORMAL("backlist[%s]", strBackIps.c_str());
    }

    IF_TRUE(conf.IsHere("support_types")) {
        HCSTRR support_types = conf.GetValue("support_types");
        setupSupportTypes(support_types);
        SLOG_NORMAL("support_types[%s]", support_types.c_str());
    }

    HRETURN_OK;
}


HCSTRR HhConfigParam::GetBConfValue (HCSTRR key) const {

    return m_conf.GetValue(key);

}


HBOOL HhConfigParam::IsBlackAddr (HCSTRR strAddr) const {

    for (size_t i = 0; i < m_back_ips.size(); ++i) {
        HCSTRR item = m_back_ips[i];
        if (strAddr.find(item) == 0) {
            return HTRUE;
        }
    }

    return HFALSE;

}


HBOOL HhConfigParam::IsSupportTypes (HCSTRR type) const {

    if (m_support_types.empty()) {
        return HTRUE;
    }

    for (size_t i = 0; i < m_support_types.size(); ++i) {
        HCSTRR item = m_support_types[i];
        if (type.find(item) == 0) {
            return HTRUE;
        }
    }

    return HFALSE;

}


HRET HhConfigParam::setupBackIp (HCSTRR ips) {

    HCStr::Split(ips, ",", m_back_ips);
    HRETURN_OK;

}


HRET HhConfigParam::setupSupportTypes (HCSTRR types) {

    HCStr::Split(types, ",", m_support_types);
    HRETURN_OK;

}
