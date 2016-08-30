/*
 *author:Alex shum
 *  date:2016-8-23
 *  file:GetAutoInviteStatusCmdHandle.cpp
 *  desc:处理自动邀请状态命令实现类
*/

#include "GetAutoInviteStatusCmdHandle.h"

GetAutoInviteStatusCmdHandle::GetAutoInviteStatusCmdHandle(void)
{
}

GetAutoInviteStatusCmdHandle::~GetAutoInviteStatusCmdHandle(void)
{
}

bool GetAutoInviteStatusCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool GetAutoInviteStatusCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetAutoInviteStatusHandle(cmdList,exit);
}

//解析命令，向服务器请求自动邀请状态
bool GetAutoInviteStatusCmdHandle::GetAutoInviteStatusHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;

	if (cmdList.size() > 0) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		bool result = g_client->GetAutoInviteMsgSwitchStatus();
		if (!result) {
			printf("getusertheme fail!\n");
		}

		isWait = result;
	}
	else {
		GetAutoInviteStatusInfo();
	}

	return isWait;
}
