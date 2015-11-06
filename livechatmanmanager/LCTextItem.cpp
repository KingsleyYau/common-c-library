/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCTextItem.h
 *   desc: LiveChat文本消息item
 */

#include "LCTextItem.h"
#include "LCMessageFilter.h"

LCTextItem::LCTextItem()
{
	m_message = "";
	m_illegal = false;
}

LCTextItem::~LCTextItem()
{

}

bool LCTextItem::Init(const string& message)
{
	m_illegal = LCMessageFilter::IsIllegalMessage(message);
	if ( m_illegal ) {
		m_message = LCMessageFilter::FilterIllegalMessage(message);
	}
	else {
		m_message = message;
	}
	return true;
}

