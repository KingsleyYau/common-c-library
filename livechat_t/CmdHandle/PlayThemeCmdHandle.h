/*
*author:Alex shum
*  date:2016-8-15
*  file:PlayThemeCmdHandle.h
*  desc:������������������� ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class PlayThemeCmdHandle :
	public ICmdHandle
{
public:
	PlayThemeCmdHandle(void);
	virtual ~PlayThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool PlayThemeHandle(list<string>& cmdList,bool &exit);//�����������������󲥷����������

};
