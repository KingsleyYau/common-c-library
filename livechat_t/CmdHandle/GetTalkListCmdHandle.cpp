#include "GetTalkListCmdHandle.h"

GetTalkListCmdHandle::GetTalkListCmdHandle(void)
{
}

GetTalkListCmdHandle::~GetTalkListCmdHandle(void)
{
}

bool GetTalkListCmdHandle::Init()
{
	return true;
}
//��������cmd����
bool GetTalkListCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetTalkListHandle(cmdList,exit);
}

//�����������TalkList
bool GetTalkListCmdHandle::GetTalkListHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;

	if (cmdList.size() > 0) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		bool result = g_client->GetTalkList(0x03);
		if (!result) {
			printf("gettalklist fail!\n");
		}

		isWait = result;
	}
	else {
		GetTalkListInfo();
	}

	return isWait;
}
