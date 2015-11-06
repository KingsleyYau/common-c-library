/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCWarningItem.h
 *   desc: LiveChat警告消息item
 */

#include "LCWarningItem.h"

LCWarningItem::LCWarningItem()
{
	m_linkMsg = "";
	m_linkOptType = Unknow;
}

LCWarningLinkItem::LCWarningLinkItem(const LCWarningLinkItem& item)
{
	m_linkMsg = item.m_linkMsg;
	m_linkOptType = item.m_linkOptType;
}

LCWarningItem::~LCWarningItem()
{

}

bool LCWarningItem::Init(const string& linkMsg, LinkOptType linkOptType)
{
	m_linkMsg = linkMsg;
	m_linkOptType = linkOptType;
	return true;
}

