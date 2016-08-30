#include "ExitCmdHandle.h"

ExitCmdHandle::ExitCmdHandle(void)
{
}

ExitCmdHandle::~ExitCmdHandle(void)
{
}

bool ExitCmdHandle::Init()
{
	return true;
}
//公共处理cmd函数,退出命令直接设置exit为true，返回false；
bool ExitCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
	exit = true;
	return false;
}
