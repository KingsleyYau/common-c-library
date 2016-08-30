/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:GetTalkListCmdHandle.h
 *  desc:����TalkList����ʵ����
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
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool GetTalkListHandle(list<string>& cmdList,bool &exit);//�����������TalkList
};
