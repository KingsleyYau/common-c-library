/*
 *author:Alex shum
 *  date:2016-8-24
 *  file:AutoInviteCmdHandle.h
 *  desc:��������/�ر��Զ�������Ϣ����ʵ���ࣨ��Ůʿ�ˣ�
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
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool AutoInviteHandle(list<string>& cmdList,bool &exit);//����������������������/�ر��Զ�������Ϣ
};
