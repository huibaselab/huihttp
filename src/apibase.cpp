/*
 * @Author: Tom Hui
 * @Date: 2019-12-18 14:55:20
 * @Description: 
 */

#include <hstr.h>
#include <hlog.h>
#include <hmutex.h>

#include "hhserver.h"
#include "apibase.h"

using namespace HUIBASE;


HCSTRR CApiBase::GetBConfValue (HCSTRR strKey) const {

    return m_pServer->GetBConfValue(strKey);

}


HRET CApiBase::Init (HCSTRR str) {

    if (str.length() > 0) {

        HNOTOK_RETURN(parseInput(str));

    }

    HRETURN_OK;
}


HRET CApiBase::parseInput(HCSTRR str) {

    HVSTRS vs;
    HNOTOK_RETURN(HCStr::Split (str, "&", vs));

    for (size_t i = 0; i < vs.size(); ++i) {

        HSTRR ss = vs[i];
        HCStr::Trim (ss);
        HVSTRS ii;

        HNOTOK_RETURN(HCStr::Split(ss, "=", ii));

        if(ii.size() < 2) {
            continue;
        }

        HSTRR skey = ii[0];
        HCStr::Trim(skey);

        HSTRR sval = ii[1];
        HCStr::Trim(sval);

        m_ins[skey] = sval;

    }

    HRETURN_OK;
}


void CApiBase::SetClientIp (HCSTRR strClientIp) {

    m_request_info.SetClientIp(strClientIp);

}


void CApiBase::SetClientPort (HINT nClientPort) {

    m_request_info.SetClientPort(nClientPort);

}


void CApiBase::SetRequestType (HCSTRR strRequestType) {

    m_request_info.SetReqestType(strRequestType);

}


CRedis* CApiBase::GetRedis (HCSTRR strRdsName) {

    CRedis* res = m_pServer->GetMem(strRdsName);
    HASSERT_THROW_MSG(res != nullptr, "redis is not config", ILL_PT);

    return res;
}


void CApiBase::AppendOutput (HCSTRR key, HCSTRR val, ResponseItemType rit) {

    ResponseItem ri(key, val, rit);
    m_res.AppendItem(ri);

}



CApiBase* CApisFactory::GetApi(HCSTRR strName){

	std::map<HSTR, api_maker>::iterator fit = m_makers.find (strName);
	if (fit == m_makers.end()) {
		return nullptr;
	}

	api_maker pfun = fit->second;

	if (pfun != nullptr) {
		return pfun();
	}

	return nullptr;

}


void CApisFactory::RegisteApi (HCSTRR strName, api_maker maker) {

	if (m_makers.find(strName) != m_makers.end()) {

		exit(-1);

	}

	m_makers.insert(std::map<HSTR, api_maker>::value_type(strName, maker) );

}
