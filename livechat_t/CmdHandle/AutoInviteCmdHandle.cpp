/*
 *author:Alex shum
 *  date:2016-8-24
 *  file:AutoInviteCmdHandle.cpp
 *  desc:��������/�ر��Զ�������Ϣ����ʵ���ࣨ��Ůʿ�ˣ�
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
//��������cmd����
bool AutoInviteCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return AutoInviteHandle(cmdList,exit);
}

//����������������������/�ر��Զ�������Ϣ
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
