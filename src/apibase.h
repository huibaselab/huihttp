/*
 * @Author: Tom Hui
 * @Date: 2019-12-18 14:55:20
 * @Description: 
 */


#ifndef __API_BASE_H__
#define __API_BASE_H__

#include <huibase.h>
#include <hsingleton.hpp>
#include <nosql/hredis.h>
#include <hdict.h>

#include "response.h"
#include "requestinfo.h"

using namespace HUIBASE;
using namespace HUIBASE::NOSQL;

class CHhServer;

class CApiBase {
 public:
    CApiBase (HCSTRR strApiName): m_strName (strApiName) { }

    virtual ~CApiBase () {}

    HCSTRR GetName () const { return m_strName; }

	HCSTRR GetBConfValue (HCSTRR strKey) const;

    void SetServer (CHhServer* pServer) { m_pServer = pServer; }

    virtual HRET Init (HCSTRR);

    virtual HRET Work()  = 0;

    const Response GetRes () const { return m_res; }

    HRET parseInput (HCSTRR str);

	const RequestInfo& GetRequestInfo() const { return m_request_info; }

public:
	void SetClientIp (HCSTRR strClientIp);

	void SetClientPort (HINT nClientPort);

	void SetRequestType (HCSTRR strRequestType);

protected:
	CRedis* GetRedis (HCSTRR strRdsName);

	void AppendOutput (HCSTRR key, HCSTRR val, ResponseItemType rit = rit_string);


 protected:
    HSTR m_strName;
	HCParam m_ins;
    Response m_res;

    CHhServer* m_pServer = nullptr;

	RequestInfo m_request_info;
};

class CApisFactory {
 public:
	typedef CApiBase* (*api_maker) ();
 public:

	CApiBase* GetApi(HCSTRR strName) ;

	void RegisteApi (HCSTRR strName, api_maker maker);


 private:
	std::map<HSTR, api_maker> m_makers;

};

typedef HCSingleton<CApisFactory> api_factory;


#define REGISTE_API(name,apiobj)                                    \
	static CApiBase* class_##name_##apiobj () {                         \
		return new apiobj(#name);                                       \
	}                                                                   \
	class __CAPICreate_##name_##apiobj {                            \
	public:                                                             \
		__CAPICreate_##name_##apiobj () {                           \
			api_factory::Instance()->RegisteApi(#name,class_##name_##apiobj); \
		}                                                               \
	};                                                                  \
	static const __CAPICreate_##name_##apiobj __creator_##name_##apiobj_maker


#endif
