/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCTextItem.h
 *   desc: LiveChat文本消息item
 */

#include "LCTextItem.h"
#include "LCMessageFilter.h"
#include <common/CheckMemoryLeak.h>

LCTextItem::LCTextItem()
{
	m_message = "";
    m_displayMsg = "";
	m_illegal = false;
}

LCTextItem::~LCTextItem()
{

}

bool LCTextItem::Init(const string& message, bool isSend)
{
    // 消息内容
    m_message = message;

    // 判断是否包含非法字符
	m_illegal = LCMessageFilter::IsIllegalMessage(message);
    
    // 显示消息内容
    if (isSend || !m_illegal) {
        m_displayMsg = message;
    }
    else {
        m_displayMsg = LCMessageFilter::FilterIllegalMessage(message);
    }
	return true;
}

