/*
 *author:Alex shum
 *  date:2016-8-24
 *  file:AutoInviteCmdHandle.cpp
 *  desc:处理启动/关闭自动邀请消息命令实现类（仅女士端）
*/
#include "AutoInviteCmdHandle.h"

AutoInviteCmdHandle::AutoInviteCmdHandle(void)
{
}

AutoInviteCmdHandle::~AutoInviteCmdHandle(void)
{
}

bool AutoInviteCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool AutoInviteCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return AutoInviteHandle(cmdList,exit);
}

//解析命令，向服务器请求启动/关闭自动邀请消息
bool AutoInviteCmdHandle::AutoInviteHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;

	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// sex
		int isOpen = atoi((*iter).c_str());

		bool result = g_client->SwitchAutoInviteMsg(isOpen == 0 ? false : true);
		if (!result) {
			printf("getusertheme fail!\n");
		}

		isWait = result;
	}
	else {
		SendAutoInviteInfo();
	}

	return isWait;
}
