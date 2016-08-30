// testCURL.cpp : 定义控制台应用程序的入口点。
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
	--- 正式环境 ---
	CL：203.169.146.66:5000
	CD：203.169.146.70:5004

	iOS：52.197.180.172:15001

	--- demo环境 ---
	CL：58.64.141.117:5000
	IDA：58.64.141.117:5001
	CD：58.64.141.117:5004
	LD：58.64.141.117:5005
*/

// LiveChat客户端
ILiveChatClient* g_client = NULL;
// LiveChat消息计算器
Counter g_msgCounter;
// 等待回调的event
HANDLE g_waitRespondEvent = NULL;

// 处理命令函数
bool HandleCmd(const char* command, bool& isWait);

LiveChatClientListener g_clientListener;

// ------ 循环处理线程函数 ------
// 循环命令启动标志
bool g_isLoopThreadRun = false;

// 循环命令队列
list_lock<LoopCmdItem>	g_loopCmdList;

// 循环文本消息启动标志
bool g_loopTextRun = false;
// 循环文本消息计数器
Counter g_loopTextCounter;
// 循环文本消息接收用户列表
list<string> g_loopTextUserList;
// 循环文本消息发送间隔时间
int g_loopTextStepTime = 5;

// 循环高表消息启动标志
bool g_loopEmotionRun = false;
// 循环高表消息计数器
Counter g_loopEmotionCounter;
// 循环高表消息接收用户列表
list<string> g_loopEmotionUserList;
// 循环高表消息发送间隔时间
int g_loopEmotionStepTime = 5;

static TH_RETURN_PARAM LoopThreadProc(void* arg)
{
	LoopCmdItem item;
	bool isLoopCmdListEmpty = true;
	while (g_isLoopThreadRun)
	{
		// 获取命令
		g_loopCmdList.lock();
		isLoopCmdListEmpty = g_loopCmdList.empty();
		if (!isLoopCmdListEmpty) {
			item = *g_loopCmdList.begin();
			g_loopCmdList.pop_front();
		}
		g_loopCmdList.unlock();

		if (!isLoopCmdListEmpty)
		{
			// 处理命令
			if (getCurrentTime() >= item.handleTime) {
				// 执行命令
				bool isWait = false;
				HandleCmd(item.cmd.c_str(), isWait);
			}
			else {
				// 命令未到时间，插入队列尾
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

	// 分割命令行
	list<string> cmdList = StringHandle::split(arg, ";");

	// 处理命令行
	bool isWait = false;
	for (list<string>::iterator iter = cmdList.begin();
		iter != cmdList.end() && !exit;
		iter++)
	{
		// 等待回调事件
		WaitForSingleObject(g_waitRespondEvent, INFINITE);

		// 打印命令
		printf("> %s\n", (*iter).c_str());

		// 处理命令
		exit = HandleCmd((*iter).c_str(), isWait);

		// 不用等待回调，则改变信号量，立即进入命令行状态
		if (!isWait) {
			SetEvent(g_waitRespondEvent);
		}
	}

	return exit;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// 初始化socket环境
	ISocketHandler::InitEnvironment();
	
	// 初始化消息计算器 (文字和表情，和票根有关)
	g_msgCounter.Init();

	// 创建并初始化 客户端
	g_client = ILiveChatClient::CreateClient();
	
	// 进入命令行状态
	if (NULL != g_client)
	{
		// 初始化
		printf("LiveChatClient is running! Please input command, 'help' print all command.\n");
		char command[2048] = {0};
		//事件对象
		g_waitRespondEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
		bool exit = false;

		// 循环处理线程
		g_isLoopThreadRun = true;
		IThreadHandler* threadHandler = IThreadHandler::CreateThreadHandler();
		threadHandler->Start(LoopThreadProc, NULL);

		// 处理main命令
		if (argc >= 2) {
			exit = HandleArg(argv[1]);
		}

		// 处理用户输入
		while (!exit)
		{
			// 等待回调事件
			WaitForSingleObject(g_waitRespondEvent, INFINITE);

			// 打印命令输入提示符
			printf("> ");

			// 用户输入命令
			gets_s(command, sizeof(command));

			// 处理命令
			bool isWait = false;
			exit = HandleCmd(command, isWait);
			if (exit) {
				break;
			}

			// 不用等待回调，则改变信号量，立即进入命令行状态
			if (!isWait) {
				SetEvent(g_waitRespondEvent);
			}
		}

		// 停止循环处理线程
		g_isLoopThreadRun = false;
		threadHandler->WaitAndStop();
		IThreadHandler::ReleaseThreadHandler(threadHandler);

		// 释放
		ILiveChatClient::ReleaseClient(g_client);
		g_client = NULL;
	}
	else 
	{
		printf("CreateClient fail!\n");
	}

	printf("shutdown Now!\n");

	// 释放socket环境
	ISocketHandler::ReleaseEnvironment();

	return 0;
}

// 处理命令函数
bool HandleCmd(const char* command, bool& isWait)
{
	bool exit = false;

	// 分割命令
	list<string> cmdList = StringHandle::split(command, " ");

	isWait = false;
	string cmd = "help";
	if(!cmdList.empty())
	{
		cmd = *cmdList.begin();
	}
	//根据不同命令，创建不同的命令处理现实类
	ICmdHandle *cmdhandle = ICmdHandle::CreateCmdHandleWithCmd(cmd);
	if(NULL != cmdhandle)
	{		
		//初始化
		cmdhandle->Init();
		//所有命令处理类的共同处理函数，里面根据不同命令处理不同
		isWait = cmdhandle->HandleTheCmd(cmdList,exit);
		// 释放命令处理器
		delete cmdhandle;
	}

	return exit;
}
