/*******************************************************
 *
 * FileName: connmap.h
 *
 * Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Wed Jun 20 16:10 2018
 *
 * Brief:
 *
 *
 *******************************************************/


#ifndef __DATACONNMAP_H__
#define __DATACONNMAP_H__

#include <huibase.h>
#include <hsingleton.hpp>
#include <db/hconnection.h>
#include <db/mysqldata.h>
#include <nosql/hredis.h>

using namespace HUIBASE;

using namespace HUIBASE::HMYSQL;
using namespace HUIBASE::NOSQL;


class DataConnMap {
 public:
    typedef std::map<HUINT, CMyConnection*> INDEX_CONNS;
    typedef std::map<HSTR, INDEX_CONNS> CONN_MAP;

   typedef std::map<HUINT, CRedis*> INDEX_REDIS;
   typedef std::map<HSTR, INDEX_REDIS> RDS_MAP;

 public:
    DataConnMap () { }

    ~ DataConnMap ();

    void AddMem (HCSTRR str, HUINT index, CRedis* redis);

    CRedis* GetMem (HCSTRR strName, HUINT index);

    void AddConn (HCSTRR str, HUINT index, CMyConnection* p);

    CMyConnection* GetConn (HCSTRR strName, HUINT index);

    HRET Query (HCSTRR strName, HUINT index, HCSTRR sql, CRes& db_res) throw ();

    HRET Exec (HCSTRR strName, HUINT index, HCSTRR sql) throw ();

    HRET Exec (HCSTRR strName, HUINT index, HCVSTRSR sqlss) throw ();

 private:
    CONN_MAP m_connmap;

    RDS_MAP m_rdsmap;
};

typedef HCSingleton<DataConnMap> DB_MAP;



#endif //__CONNMAP_H__
