#include "GetUserThemeCmdHandle.h"

GetUserThemeCmdHandle::GetUserThemeCmdHandle(void)
{
}

GetUserThemeCmdHandle::~GetUserThemeCmdHandle(void)
{
}

bool GetUserThemeCmdHandle::Init()
{
	return true;
}
//��������cmd����
bool GetUserThemeCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetUserThemeHandle(cmdList,exit);
}

//������������ȡ��/Ůʿ�����
bool GetUserThemeCmdHandle::GetUserThemeHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;

	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		bool result = g_client->GetPaidTheme(userId);
		if (!result) {
			printf("getusertheme fail!\n");
		}

		isWait = result;
	}
	else {
		GetUserThemeInfo();
	}

	return isWait;
}
