/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCWarningItem.h
 *   desc: LiveChat警告消息item
 */

#include "LCWarningItem.h"

LCWarningItem::LCWarningItem()
{
	m_message = "";
}

LCWarningItem::~LCWarningItem()
{

}

bool LCWarningItem::Init(const string& message)
{
	m_message = message;
	return true;
}

bool LCWarningItem::Init(const string& message, const LCWarningLinkItem& linkItem)
{
	m_message = message;
	m_linkType = linkType;
	return true;
}
