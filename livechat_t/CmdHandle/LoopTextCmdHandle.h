/*
*author:Alex shum
*  date:2016-8-15
*  file:LoopTextCmdHandle.h
*  desc:循环发送消息处理命令，由looptext转变为looptextproc命令,由loop线程把looptextproc转到总命令处理函数handlecmd处
*/
#pragma once
#include "ICmdhandle.h"

class LoopTextCmdHandle :
	public ICmdHandle
{
public:
	LoopTextCmdHandle(void);
	virtual ~LoopTextCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool SendLoopTextHandle(list<string>& cmdList,bool &exit);//发送消息
};
