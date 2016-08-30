#include "GetAllThemeCmdHandle.h"

GetAllThemeCmdHandle::GetAllThemeCmdHandle(void)
{
}

GetAllThemeCmdHandle::~GetAllThemeCmdHandle(void)
{
}

bool GetAllThemeCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool GetAllThemeCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetAllThemeHandle(cmdList,exit);
}

//向服务器获取所有主题包
bool GetAllThemeCmdHandle::GetAllThemeHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;

	if (cmdList.size() > 0) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		bool result = g_client->GetAllPaidTheme();
		if (!result) {
			printf("getusertheme fail!\n");
		}

		isWait = result;
	}
	else {
		GetAllThemeInfo();
	}

	return isWait;
}
