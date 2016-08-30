/*
*author:Alex shum
*  date:2016-8-15
*  file:LoopEmotionCmdHandle.h
*  desc:循环发送高级表情消息处理命令，由loopemotion变为loopemotionproc命令,由loop线程把loopemotionproc转到总命令处理函数handlecmd处
*/
#pragma once
#include "ICmdhandle.h"

class LoopEmotionCmdHandle :
	public ICmdHandle
{
public:
	LoopEmotionCmdHandle(void);
	virtual ~LoopEmotionCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool SendLoopEmotionHandle(list<string>& cmdList,bool &exit);//处理循环高级表情命令，但这函数没有发送
	//void InsertLoopEmotionProcCmd(const list<string>& userList);

};
