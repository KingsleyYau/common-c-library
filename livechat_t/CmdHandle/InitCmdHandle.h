/*
 * author:Alex shum
 *   date:2016-8-12
 *   file:InitCmdHandle
 *   desc:�����ʼ������õ�����˵�ip�Ͷ˿�
*/
#pragma once
#include "ICmdHandle.h"

using namespace std;

class InitCmdHandle :
	public ICmdHandle
{
public:
	InitCmdHandle(void);
	virtual ~InitCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool InitHandle(list<string>& cmdList,bool &exit);
};
