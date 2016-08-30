/*
*author:Alex shum
*  date:2016-8-15
*  file:EmotionCmdHandle.h
*  desc:处理高级消息命令实现类
*/
#pragma once
#include "ICmdhandle.h"

class EmotionCmdHandle :
	public ICmdHandle
{
public:
	EmotionCmdHandle(void);
	virtual ~EmotionCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool SendEmotionHandle(list<string>& cmdList,bool &exit);//发送高级表情消息处理
};
