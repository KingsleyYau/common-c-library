/*
 *author:Alex shum
 *  date:2016-8-24
 *  file:AutoInviteCmdHandle.h
 *  desc:处理启动/关闭自动邀请消息命令实现类（仅女士端）
*/

#pragma once
#include "ICmdhandle.h"

class AutoInviteCmdHandle :
	public ICmdHandle
{
public:
	AutoInviteCmdHandle(void);
	virtual ~AutoInviteCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool AutoInviteHandle(list<string>& cmdList,bool &exit);//解析命令，向服务器请求启动/关闭自动邀请消息
};
