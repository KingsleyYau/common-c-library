/*
 *author:Alex shum
 *  date:2016-8-23
 *  file:GetAutoInviteStatusCmdHandle.h
 *  desc:�����Զ�����״̬����ʵ����
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
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool GetAutoInviteStatusHandle(list<string>& cmdList,bool &exit);//�������������������Զ�����״̬
};
