/*
 * @Author: Tom Hui
 * @Date: 2019-12-18 14:55:20
 * @Description: 
 */


#ifndef __REPONSE_H__
#define __REPONSE_H__

#include <huibase.h>

using namespace HUIBASE;

typedef enum {
	rit_normal,
	rit_string,
	rit_object,
	rit_end
} ResponseItemType;

class ResponseItem {
public:
	ResponseItem () { }	

	ResponseItem (HCSTRR name, HCSTRR value, ResponseItemType type = rit_string);

	~ ResponseItem() { }

public:
	HCSTRR GetName () const { return m_name; }

	HCSTRR GetValue () const { return m_value; }

	ResponseItemType GetType () const { return m_type; }

	HSTR AsItem () const;

private:
	HSTR m_name;
	HSTR m_value;
	ResponseItemType m_type;
};

class Response {	
public:
	Response () { }
	~ Response()  { }

public:
	HBOOL Nonresponse () const;

	void AppendItem (const ResponseItem& item);

	HSTR GetResponse () const;

private:
	std::vector<ResponseItem> m_items;
};

#endif
