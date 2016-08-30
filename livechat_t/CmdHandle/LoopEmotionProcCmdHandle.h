/*
*author:Alex shum
*  date:2016-8-15
*  file:LoopEmotionProcCmdHandle.h
*  desc:循环发送高级表情消息处理命令实现类
*/

#pragma once
#include "ICmdhandle.h"

class LoopEmotionProcCmdHandle :
	public ICmdHandle
{
public:
	LoopEmotionProcCmdHandle(void);
	virtual ~LoopEmotionProcCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool SendLoopEmotionProc(list<string>& cmdList,bool &exit);//循环发送高级表情处理

};
