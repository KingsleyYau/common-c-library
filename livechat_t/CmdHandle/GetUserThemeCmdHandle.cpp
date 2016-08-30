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
//公共处理cmd函数
bool GetUserThemeCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetUserThemeHandle(cmdList,exit);
}

//向服务器请求获取男/女士主题包
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
