/*
 *author:Alex shum
 *  date:2016-8-15
 *  file:ApplyThemeCmdHandle.h
 *  desc:����Ӧ�����������ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class ApplyThemeCmdHandle :
	public ICmdHandle
{
public:
	ApplyThemeCmdHandle(void);
	virtual ~ApplyThemeCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd�������������
private:
	bool ApplyThemeHandle(list<string>& cmdList,bool &exit);//������������������Ӧ�������
};
