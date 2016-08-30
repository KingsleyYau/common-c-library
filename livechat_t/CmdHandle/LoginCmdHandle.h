/*
 *author:Alex shum
 *  date:2016-8-12
 *  file:LoginCmdHandle.h
 *  desc:�����¼�����Ҫ�õ��˺����룬���ͷ��������
*/
#pragma once
#include "ICmdHandle.h"

using namespace std;

class LoginCmdHandle :
	public ICmdHandle
{
public:
	LoginCmdHandle(void);
	virtual ~LoginCmdHandle(void);
public:
	virtual bool Init();
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit);//��������cmd����
private:
	bool LoginHandle(list<string>& cmdList,bool &exit);
};
