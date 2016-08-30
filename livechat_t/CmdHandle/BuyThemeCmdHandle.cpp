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
//��������cmd����
bool BuyThemeCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return BuyThemeHandle(cmdList,exit);
}

//���������������������������
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
