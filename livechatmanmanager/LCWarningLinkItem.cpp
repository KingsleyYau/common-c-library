/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCWarningLinkItem.cpp
 *   desc: LiveChat警告消息item
 */

#include "LCWarningLinkItem.h"
#include <common/CheckMemoryLeak.h>

LCWarningLinkItem::LCWarningLinkItem()
{
	m_linkMsg = "";
	m_linkOptType = Unknow;
}

LCWarningLinkItem::LCWarningLinkItem(const LCWarningLinkItem& item)
{
	m_linkMsg = item.m_linkMsg;
	m_linkOptType = item.m_linkOptType;
}

LCWarningLinkItem::~LCWarningLinkItem()
{

}

bool LCWarningLinkItem::Init(const string& linkMsg, LinkOptType linkOptType)
{
	m_linkMsg = linkMsg;
	m_linkOptType = linkOptType;
	return true;
}

