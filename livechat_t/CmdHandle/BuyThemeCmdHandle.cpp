#include "BuyThemeCmdHandle.h"

BuyThemeCmdHandle::BuyThemeCmdHandle(void)
{
}

BuyThemeCmdHandle::~BuyThemeCmdHandle(void)
{
}

bool BuyThemeCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool BuyThemeCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return BuyThemeHandle(cmdList,exit);
}

//解析命令，向想服务器请求购买主题包
bool BuyThemeCmdHandle::BuyThemeHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;

	if (cmdList.size() > 2) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// themeId
		string themeId = (*iter);
		iter++;

		bool result = g_client->ManFeeTheme(userId, themeId);
		if (!result) {
			printf("buytheme fail!\n");
		}

		isWait = result;
	}
	else {
		BuyThemeInfo();
	}

	return isWait;
}
