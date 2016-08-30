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
//公共处理cmd函数
bool GetTalkListCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetTalkListHandle(cmdList,exit);
}

//向服务器请求TalkList
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
