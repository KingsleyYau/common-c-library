/*
*author:Alex shum
*  date:2016-8-15
*  file:LooptextProcCmdHandle.h
*  desc:循环发送消息处理命令实现类
*/


#pragma once
#include "ICmdhandle.h"

class LoopTextProcCmdHandle :
	public ICmdHandle
{
public:
	LoopTextProcCmdHandle(void);
	virtual ~LoopTextProcCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool SendLoopTextProc(list<string>& cmdList,bool &exit);//循环发送消息
};
