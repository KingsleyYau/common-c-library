/*
*author:Alex shum
*  date:2016-8-15
*  file:UserInfoCmdHandle.h
*  desc:�����ȡ��/Ůʿ��������� ʵ����
*/


#pragma once
#include "ICmdhandle.h"

class UserInfoCmdHandle :
	public ICmdHandle
{
public:
	UserInfoCmdHandle(void);
	virtual ~UserInfoCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool GetUserInfoHandle(list<string>& cmdList,bool &exit);//�������������������ȡ��/Ůʿ�����

};
