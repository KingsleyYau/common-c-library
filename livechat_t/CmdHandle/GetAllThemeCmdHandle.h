/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:GetAllThemeCmdHandle.h
 *  desc:�����ȡ������������ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class GetAllThemeCmdHandle :
	public ICmdHandle
{
public:
	GetAllThemeCmdHandle(void);
	virtual ~GetAllThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool GetAllThemeHandle(list<string>& cmdList,bool &exit);//���������ȡ���������
};
