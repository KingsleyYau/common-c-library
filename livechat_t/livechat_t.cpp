// testCURL.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <livechat/ILiveChatClient.h>
#include <common/StringHandle.h>
#include <livechat/ISocketHandler.h>
#include <livechat/Counter.h>
#include <string>
#include <livechat/IThreadHandler.h>
#include <common/list_lock.h>
#include <common/CommonFunc.h>
#include "CmdHandle/ICmdHandle.h"

using namespace std;

/*
	--- ��ʽ���� ---
	CL��203.169.146.66:5000
	CD��203.169.146.70:5004

	iOS��52.197.180.172:15001

	--- demo���� ---
	CL��58.64.141.117:5000
	IDA��58.64.141.117:5001
	CD��58.64.141.117:5004
	LD��58.64.141.117:5005
*/

// LiveChat�ͻ���
ILiveChatClient* g_client = NULL;
// LiveChat��Ϣ������
Counter g_msgCounter;
// �ȴ��ص���event
HANDLE g_waitRespondEvent = NULL;

// ���������
bool HandleCmd(const char* command, bool& isWait);

LiveChatClientListener g_clientListener;

// ------ ѭ�������̺߳��� ------
// ѭ������������־
bool g_isLoopThreadRun = false;

// ѭ���������
list_lock<LoopCmdItem>	g_loopCmdList;

// ѭ���ı���Ϣ������־
bool g_loopTextRun = false;
// ѭ���ı���Ϣ������
Counter g_loopTextCounter;
// ѭ���ı���Ϣ�����û��б�
list<string> g_loopTextUserList;
// ѭ���ı���Ϣ���ͼ��ʱ��
int g_loopTextStepTime = 5;

// ѭ���߱���Ϣ������־
bool g_loopEmotionRun = false;
// ѭ���߱���Ϣ������
Counter g_loopEmotionCounter;
// ѭ���߱���Ϣ�����û��б�
list<string> g_loopEmotionUserList;
// ѭ���߱���Ϣ���ͼ��ʱ��
int g_loopEmotionStepTime = 5;

static TH_RETURN_PARAM LoopThreadProc(void* arg)
{
	LoopCmdItem item;
	bool isLoopCmdListEmpty = true;
	while (g_isLoopThreadRun)
	{
		// ��ȡ����
		g_loopCmdList.lock();
		isLoopCmdListEmpty = g_loopCmdList.empty();
		if (!isLoopCmdListEmpty) {
			item = *g_loopCmdList.begin();
			g_loopCmdList.pop_front();
		}
		g_loopCmdList.unlock();

		if (!isLoopCmdListEmpty)
		{
			// ��������
			if (getCurrentTime() >= item.handleTime) {
				// ִ������
				bool isWait = false;
				HandleCmd(item.cmd.c_str(), isWait);
			}
			else {
				// ����δ��ʱ�䣬�������β
				g_loopCmdList.lock();
				g_loopCmdList.push_back(item);
				g_loopCmdList.unlock();
			}
		}
		Sleep(1000);
	}

	return 0;
}

bool HandleArg(const char* arg)
{
	bool exit = false;

	// �ָ�������
	list<string> cmdList = StringHandle::split(arg, ";");

	// ����������
	bool isWait = false;
	for (list<string>::iterator iter = cmdList.begin();
		iter != cmdList.end() && !exit;
		iter++)
	{
		// �ȴ��ص��¼�
		WaitForSingleObject(g_waitRespondEvent, INFINITE);

		// ��ӡ����
		printf("> %s\n", (*iter).c_str());

		// ��������
		exit = HandleCmd((*iter).c_str(), isWait);

		// ���õȴ��ص�����ı��ź�������������������״̬
		if (!isWait) {
			SetEvent(g_waitRespondEvent);
		}
	}

	return exit;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// ��ʼ��socket����
	ISocketHandler::InitEnvironment();
	
	// ��ʼ����Ϣ������ (���ֺͱ��飬��Ʊ���й�)
	g_msgCounter.Init();

	// ��������ʼ�� �ͻ���
	g_client = ILiveChatClient::CreateClient();
	
	// ����������״̬
	if (NULL != g_client)
	{
		// ��ʼ��
		printf("LiveChatClient is running! Please input command, 'help' print all command.\n");
		char command[2048] = {0};
		//�¼�����
		g_waitRespondEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		bool exit = false;

		// ѭ�������߳�
		g_isLoopThreadRun = true;
		IThreadHandler* threadHandler = IThreadHandler::CreateThreadHandler();
		threadHandler->Start(LoopThreadProc, NULL);

		// ����main����
		if (argc >= 2) {
			exit = HandleArg(argv[1]);
		}

		// �����û�����
		while (!exit)
		{
			// �ȴ��ص��¼�
			WaitForSingleObject(g_waitRespondEvent, INFINITE);

			// ��ӡ����������ʾ��
			printf("> ");

			// �û���������
			gets_s(command, sizeof(command));

			// ��������
			bool isWait = false;
			exit = HandleCmd(command, isWait);
			if (exit) {
				break;
			}

			// ���õȴ��ص�����ı��ź�������������������״̬
			if (!isWait) {
				SetEvent(g_waitRespondEvent);
			}
		}

		// ֹͣѭ�������߳�
		g_isLoopThreadRun = false;
		threadHandler->WaitAndStop();
		IThreadHandler::ReleaseThreadHandler(threadHandler);

		// �ͷ�
		ILiveChatClient::ReleaseClient(g_client);
		g_client = NULL;
	}
	else 
	{
		printf("CreateClient fail!\n");
	}

	printf("shutdown Now!\n");

	// �ͷ�socket����
	ISocketHandler::ReleaseEnvironment();

	return 0;
}

// ���������
bool HandleCmd(const char* command, bool& isWait)
{
	bool exit = false;

	// �ָ�����
	list<string> cmdList = StringHandle::split(command, " ");

	isWait = false;
	string cmd = "help";
	if(!cmdList.empty())
	{
		cmd = *cmdList.begin();
	}
	//���ݲ�ͬ���������ͬ���������ʵ��
	ICmdHandle *cmdhandle = ICmdHandle::CreateCmdHandleWithCmd(cmd);
	if(NULL != cmdhandle)
	{		
		//��ʼ��
		cmdhandle->Init();
		//�����������Ĺ�ͬ��������������ݲ�ͬ�����ͬ
		isWait = cmdhandle->HandleTheCmd(cmdList,exit);
		// �ͷ��������
		delete cmdhandle;
	}

	return exit;
}
