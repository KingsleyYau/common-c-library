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
//��������cmd����,�˳�����ֱ������exitΪtrue������false��
bool ExitCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
	exit = true;
	return false;
}
