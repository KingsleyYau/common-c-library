/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCSystemItem.h
 *   desc: LiveChat系统消息item
 */

#include "LCSystemItem.h"
#include <common/CheckMemoryLeak.h>

LCSystemItem::LCSystemItem()
{
	m_codeType = MESSAGE;
	m_message = "";
}

LCSystemItem::~LCSystemItem()
{

}

bool LCSystemItem::Init(const string& message)
{
	m_codeType = MESSAGE;
	m_message = message;
	return true;
}
