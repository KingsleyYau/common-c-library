#include "InitCmdHandle.h"
#include "common/StringHandle.h"

InitCmdHandle::InitCmdHandle(void)
{
}

InitCmdHandle::~InitCmdHandle(void)
{
}

bool InitCmdHandle::Init()
{
	return true;
}

bool InitCmdHandle::HandleTheCmd(list<string>& cmdList,bool &exit)
{
      return InitHandle(cmdList,exit);
}
bool InitCmdHandle::InitHandle(list<string>& cmdList,bool &exit)
{
	bool wait = false;
	if(cmdList.size() == 3)
	{
		list<string>::const_iterator iter = cmdList.begin();
		iter++;
		
		string ipListCmd = (*iter);
		//将ipListCmd分到队列中，分析init命令输入几个服务器ip
		list<string> ipList = StringHandle::split(ipListCmd,",");
		iter++;

		int port = atoi((*iter).c_str());

	    if (g_client->Init(ipList, port, &g_clientListener)) {
			printf("init success!\n");
		}
		else {
			printf("init fail!\n");
		}
	}
	else
	{
		InitInfo();
	}
	return wait;
}