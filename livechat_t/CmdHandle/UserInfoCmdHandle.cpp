#include "UserInfoCmdHandle.h"

UserInfoCmdHandle::UserInfoCmdHandle(void)
{
}

UserInfoCmdHandle::~UserInfoCmdHandle(void)
{
}
bool UserInfoCmdHandle::Init()
{
	return true;
}

//公共处理cmd函数
bool UserInfoCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return GetUserInfoHandle(cmdList,exit);
}

//解析命令，向服务器请求获取男/女士主题包
bool UserInfoCmdHandle::GetUserInfoHandle(list<string>& cmdList,bool &exit)
{
	bool isWait = true;
	if (cmdList.size() > 1) 
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;

		// userId
		string userId = (*iter);
		iter++;

		bool result = g_client->GetUserInfo(userId);
		if (!result) {
			printf("userinfo fail!\n");
		}
	}
	else {
		GetUserInfo();
		isWait = false;
	}
	return isWait;
}