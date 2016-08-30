/*
 *author:Alex shum
 *  date:2016-8-23
 *  file:GetAutoInviteStatusCmdHandle.cpp
 *  desc:�����Զ�����״̬����ʵ����
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
//��������cmd����
bool GetAutoInviteStatusCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetAutoInviteStatusHandle(cmdList,exit);
}

//�������������������Զ�����״̬
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
