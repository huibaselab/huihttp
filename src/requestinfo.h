/*
 * @Author: Tom Hui
 * @Date: 2019-12-18 14:55:20
 * @Description: 
 */


#ifndef __REQUEST_INFO_H__
#define __REQUEST_INFO_H__

#include <huibase.h>

using namespace HUIBASE;


class RequestInfo {	
public:
	RequestInfo () { }
	~ RequestInfo()  { }

public:
	void SetClientIp (HCSTRR strClientIp) { m_strClientIp = strClientIp; }

	HCSTRR GetClientIp () const { return m_strClientIp; }

	void SetClientPort ( HINT nClientPort) { m_nClientPort = nClientPort; }

	HINT GetClientPort () const { return m_nClientPort; }

	void SetReqestType (HCSTRR strRequestType) { m_strRequestType = strRequestType; }

	HCSTRR GetRequestType () const { return m_strRequestType; }


private:
	HSTR m_strClientIp;

	HINT m_nClientPort;

	HSTR m_strRequestType;

};

#endif
