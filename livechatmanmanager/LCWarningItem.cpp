/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCWarningItem.h
 *   desc: LiveChat警告消息item
 */

#include "LCWarningItem.h"
#include <common/CheckMemoryLeak.h>

LCWarningItem::LCWarningItem()
{
	m_codeType = MESSAGE;
	m_message = "";
	m_linkItem = NULL;
}

LCWarningItem::~LCWarningItem()
{

}

bool LCWarningItem::Init(const string& message)
{
	m_codeType = MESSAGE;
	m_message = message;
	return true;
}

bool LCWarningItem::Init(const string& message, LCWarningLinkItem* linkItem)
{
	m_codeType = MESSAGE;
	m_message = message;
	m_linkItem = linkItem;
	return true;
}
