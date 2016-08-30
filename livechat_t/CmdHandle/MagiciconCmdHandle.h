/*
*author:Alex shum
*  date:2016-8-15
*  file:MagiciconCmdHandle.h
*  desc:处理小表情命令实现类
*/

#pragma once
#include "ICmdhandle.h"

class MagiciconCmdHandle :
	public ICmdHandle
{
public:
	MagiciconCmdHandle(void);
	virtual ~MagiciconCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool SendMagicIconHandle(list<string>& cmdList,bool &exit);//解析命令，发送小表情
};
