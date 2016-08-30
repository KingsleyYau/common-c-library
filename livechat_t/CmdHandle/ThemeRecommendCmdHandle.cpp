/*
 *author:Alex shum
 *  date:2016-8-24
 *  file:ThemeRecommendCmdHandle.cpp
 *  desc:����Ůʿ�Ƽ���ʿ�������������ʵ����
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
//��������cmd����
bool ThemeRecommendCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return ThemeRecommendHandle(cmdList,exit);
}

//������������������Ůʿ�Ƽ���ʿ���������
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
