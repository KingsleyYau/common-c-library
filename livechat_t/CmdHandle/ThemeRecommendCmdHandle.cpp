/*
 *author:Alex shum
 *  date:2016-8-24
 *  file:ThemeRecommendCmdHandle.cpp
 *  desc:处理女士推荐男士购买主题包命令实现类
*/
#include "ThemeRecommendCmdHandle.h"

ThemeRecommendCmdHandle::ThemeRecommendCmdHandle(void)
{
}

ThemeRecommendCmdHandle::~ThemeRecommendCmdHandle(void)
{
}

bool ThemeRecommendCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool ThemeRecommendCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return ThemeRecommendHandle(cmdList,exit);
}

//解析命令，向服务器请求女士推荐男士购买主题包
bool ThemeRecommendCmdHandle::ThemeRecommendHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = false;

	if (cmdList.size() > 0) 
	{

		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		// themeId
		string themeId = (*iter);
		iter++;

		bool result = g_client->RecommendThemeToMan(userId, themeId);
		if (!result) {
			printf("ThemeRecommend fail!\n");
		}

	}
	else {
		SendThemeRecommendInfo();
	}

	return isWait;
}
