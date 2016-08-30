/*
*author:Alex shum
*  date:2016-8-15
*  file:CmDHandleDef.h
*  desc:���������ࣨmain��Ҫ�õ���һЩ�ṹ�壬����д��һ�𷽱����
*/

#pragma once
#include <common/list_lock.h>
#include <string>
#include "livechat/ILiveChatClient.h"
#include "livechat/Counter.h"
#include "livechat/ILiveChatClientDef.h"
#include <list>

typedef struct _tagLoopCmdItem
{
	string cmd;
	long long handleTime;
	_tagLoopCmdItem() {
		handleTime = 0;
	}
	_tagLoopCmdItem(const struct _tagLoopCmdItem& item) {
		cmd = item.cmd;
		handleTime = item.handleTime;
	}
} LoopCmdItem;

