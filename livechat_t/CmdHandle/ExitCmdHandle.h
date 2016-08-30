/*
 *aythor:Alex shum
 *  date:2016-8-12
 *  file:ExitCmdHandle
 *  desc:处理退出命令实现类，打印exit
*/
#pragma once
#include "ICmdHandle.h"

using namespace std;
class ExitCmdHandle : public ICmdHandle
{
public:
	ExitCmdHandle(void);
	virtual ~ExitCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
};
