/*
 *author: Alex shum
 *  date: 2016-8-12
 *  file: ICmdHamdle
 *  desc: ��������ӿ��࣬��ҪΪ�˼���livechat_t.cpp�Ĵ�����
 */
#pragma once
#include "CmdHandleDef.h"
#include "CommonFuncCmdHandle.h"
#include "livechat_t/LiveChatClientListener.h"
using namespace std;

// LiveChat�ͻ���
extern ILiveChatClient* g_client;
// LiveChat��Ϣ������
extern Counter g_msgCounter;
// �ȴ��ص���event
extern HANDLE g_waitRespondEvent;

extern LiveChatClientListener g_clientListener;

// ------ ѭ�������̺߳��� ------
// ѭ������������־
extern bool g_isLoopThreadRun;

// ѭ���������
extern list_lock<LoopCmdItem>	g_loopCmdList;

// ѭ���ı���Ϣ������־
extern bool g_loopTextRun;
// ѭ���ı���Ϣ������
extern Counter g_loopTextCounter;
// ѭ���ı���Ϣ�����û��б�
extern list<string> g_loopTextUserList;
// ѭ���ı���Ϣ���ͼ��ʱ��
extern int g_loopTextStepTime;

// ѭ���߱���Ϣ������־
extern bool g_loopEmotionRun;
// ѭ���߱���Ϣ������
extern Counter g_loopEmotionCounter;
// ѭ���߱���Ϣ�����û��б�
extern list<string> g_loopEmotionUserList;
// ѭ���߱���Ϣ���ͼ��ʱ��
extern int g_loopEmotionStepTime;

class ICmdHandle
{
public:
	static ICmdHandle* CreateCmdHandleWithCmd(string cmd);

public:
	ICmdHandle(void){};
	virtual ~ICmdHandle(void){};
public:
	virtual bool Init() = 0;
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit) = 0;//��������cmd����
};