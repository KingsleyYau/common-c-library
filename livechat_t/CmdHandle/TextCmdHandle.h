/*
*author:Alex shum
*  date:2016-8-15
*  file:TextCmdHandle.h
*  desc:处理发送消息命令
*/
#pragma once
#include "ICmdhandle.h"

class TextCmdHandle :
	public ICmdHandle
{
public:
	TextCmdHandle(void);
	virtual ~TextCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool SendTextHandle(list<string>& cmdList,bool &exit);//发送消息处理
};
