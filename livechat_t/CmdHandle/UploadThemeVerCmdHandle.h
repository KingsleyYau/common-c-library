/*
*author:Alex shum
*  date:2016-8-15
*  file:UploadThemeVerCmdHandle.h
*  desc:�����ϴ�������б�汾������ ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class UploadThemeVerCmdHandle :
	public ICmdHandle
{
public:
	UploadThemeVerCmdHandle(void);
	virtual ~UploadThemeVerCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool UploadThemeVerHandle(list<string>& cmdList,bool &exit);//�������������������ϴ�������б�汾��
};
