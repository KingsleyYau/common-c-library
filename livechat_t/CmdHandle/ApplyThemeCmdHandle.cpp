#include "ApplyThemeCmdHandle.h"

ApplyThemeCmdHandle::ApplyThemeCmdHandle(void)
{
}

ApplyThemeCmdHandle::~ApplyThemeCmdHandle(void)
{
}

bool ApplyThemeCmdHandle::Init()
{
	return true;
}
//��������cmd����
bool ApplyThemeCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return ApplyThemeHandle(cmdList,exit);
}

//������������������Ӧ�������
bool ApplyThemeCmdHandle::ApplyThemeHandle(list<string>& cmdList,bool &exit)
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

		bool result = g_client->ManApplyTheme(userId, themeId);
		if (!result) {
			printf("applytheme fail!\n");
		}

		isWait = result;
	}
	else {
		ApplyThemeInfo();
	}

	return isWait;
}