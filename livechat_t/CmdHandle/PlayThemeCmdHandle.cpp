#include "PlayThemeCmdHandle.h"

PlayThemeCmdHandle::PlayThemeCmdHandle(void)
{
}

PlayThemeCmdHandle::~PlayThemeCmdHandle(void)
{
}

bool PlayThemeCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool PlayThemeCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return PlayThemeHandle(cmdList,exit);
}

//解析命令，向服务器请求播放主题包动画
bool PlayThemeCmdHandle::PlayThemeHandle(list<string>& cmdList,bool &exit)
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

		bool result = g_client->PlayThemeMotion(userId, themeId);
		if (!result) {
			printf("playtheme fail!\n");
		}

		isWait = result;
	}
	else {
		PlayThemeInfo();
	}
	return isWait;
}
