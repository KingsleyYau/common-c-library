/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:GetUserThemeCmdHandle.h
 *  desc:�����ȡ��/Ůʿ���������ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class GetUserThemeCmdHandle :
	public ICmdHandle
{
public:
	GetUserThemeCmdHandle(void);
	virtual ~GetUserThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool GetUserThemeHandle(list<string>& cmdList,bool &exit);//������������ȡ��/Ůʿ�����
};
