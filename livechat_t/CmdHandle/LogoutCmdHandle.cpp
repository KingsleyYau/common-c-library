#include "LogoutCmdHandle.h"

LogoutCmdHandle::LogoutCmdHandle(void)
{
}

LogoutCmdHandle::~LogoutCmdHandle(void)
{
}

bool LogoutCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数
bool LogoutCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return LogoutHandle(cmdList,exit);
}
bool LogoutCmdHandle::LogoutHandle(list<string>& cmdList,bool &exit)
{
	bool wait = true;
	g_client->Logout();
	return wait;
}
