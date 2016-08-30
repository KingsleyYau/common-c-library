/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:GetTalkListCmdHandle.h
 *  desc:处理TalkList命令实现类
*/

#pragma once
#include "ICmdhandle.h"

class GetTalkListCmdHandle :
	public ICmdHandle
{
public:
	GetTalkListCmdHandle(void);
	virtual ~GetTalkListCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//公共处理cmd函数
private:
	bool GetTalkListHandle(list<string>& cmdList,bool &exit);//向服务器请求TalkList
};
