/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCSystemItem.h
 *   desc: LiveChat系统消息item
 */

#include "LCSystemItem.h"

LCSystemItem::LCSystemItem()
{
	m_message = "";
}

LCSystemItem::~LCSystemItem()
{

}

bool LCSystemItem::Init(const string& message)
{
	m_message = message;
	return true;
}
