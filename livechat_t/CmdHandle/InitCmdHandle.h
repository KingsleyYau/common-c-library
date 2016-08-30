/*
 * author:Alex shum
 *   date:2016-8-12
 *   file:InitCmdHandle
 *   desc:处理初始化命令，得到服务端的ip和端口
*/
#pragma once
#include "ICmdHandle.h"

using namespace std;

class InitCmdHandle :
	public ICmdHandle
{
public:
	InitCmdHandle(void);
	virtual ~InitCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool InitHandle(list<string>& cmdList,bool &exit);
};
