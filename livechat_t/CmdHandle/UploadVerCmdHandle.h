/*
*author:Alex shum
*  date:2016-8-15
*  file:UploadVerCmdHandle.h
*  desc:�����ϴ��汾������ ʵ����
*/

#pragma once
#include "ICmdhandle.h"

class UploadVerCmdHandle :
	public ICmdHandle
{
public:
	UploadVerCmdHandle(void);
	virtual ~UploadVerCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool UploadVerHandle(list<string>& cmdList,bool &exit);//�����������������󴫰汾��
};
