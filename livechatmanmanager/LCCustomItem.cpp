/*
 * author: Samson.Fan
 *   date: 2016-05-28
 *   file: LCCustomItem.h
 *   desc: LiveChat系统消息item
 */

#include "LCCustomItem.h"
#include <common/CheckMemoryLeak.h>

LCCustomItem::LCCustomItem()
{
	m_param = 0;
}

LCCustomItem::~LCCustomItem()
{

}

bool LCCustomItem::Init(long param)
{
	m_param = param;
	return true;
}
