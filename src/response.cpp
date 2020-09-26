/*
 * @Author: Tom Hui
 * @Date: 2019-12-18 14:55:20
 * @Description: 
 */

#include "response.h"
#include <hstr.h>

ResponseItem::ResponseItem (HCSTRR name, HCSTRR value, ResponseItemType type) 
    : m_name (name), m_value(value), m_type(type) {

}


HSTR ResponseItem::AsItem () const {

    HSTR res;
    switch(GetType()) {
        case rit_string:
            res = HCStr::Format("\"%s\"", GetValue().c_str());
        break;
        case rit_normal:
        case rit_object:
            res = HCStr::Format("%s", GetValue().c_str());
        break;
        default:
        break;
    }

    return res;

}


HBOOL Response::Nonresponse () const {

    HRET_BOOL(m_items.empty());

}


void Response::AppendItem (const ResponseItem& item) {

    m_items.push_back(item);

}



HSTR Response::GetResponse () const {

    if (m_items.size() == 1) {

        const ResponseItem& item = m_items[0];
        return item.AsItem();

    } 

    stringstream ss;
    ss << "{";
    for (HSIZE i = 0; i < m_items.size(); ++i) {

        const ResponseItem& item = m_items[i];
        ss << "\"" << item.GetName() << "\": " << item.AsItem() << " ,";

    }

    HSTR res = ss.str();
    res = res.substr(0, res.length() - 1);
    res += "}";

    return res;
}