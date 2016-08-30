/*
 *author:Alex shum
 *  date:2016-8-23
 *  file:GetAutoInviteStatusCmdHandle.h
 *  desc:处理自动邀请状态命令实现类
*/

#pragma once
#include "ICmdhandle.h"

class GetAutoInviteStatusCmdHandle :
	public ICmdHandle
{
public:
	GetAutoInviteStatusCmdHandle(void);
	virtual ~GetAutoInviteStatusCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool GetAutoInviteStatusHandle(list<string>& cmdList,bool &exit);//解析命令，向服务器请求自动邀请状态
};
