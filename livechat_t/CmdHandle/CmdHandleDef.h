/*
*author:Alex shum
*  date:2016-8-15
*  file:CmDHandleDef.h
*  desc:处理命令类（main）要用到的一些结构体，类型写到一起方便调用
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

