/*
 *author: Alex shum
 *  date: 2016-8-12
 *  file: ICmdHamdle
 *  desc: 处理命令接口类，主要为了减少livechat_t.cpp的代码量
 */
#pragma once
#include "CmdHandleDef.h"
#include "CommonFuncCmdHandle.h"
#include "livechat_t/LiveChatClientListener.h"
using namespace std;

// LiveChat客户端
extern ILiveChatClient* g_client;
// LiveChat消息计算器
extern Counter g_msgCounter;
// 等待回调的event
extern HANDLE g_waitRespondEvent;

extern LiveChatClientListener g_clientListener;

// ------ 循环处理线程函数 ------
// 循环命令启动标志
extern bool g_isLoopThreadRun;

// 循环命令队列
extern list_lock<LoopCmdItem>	g_loopCmdList;

// 循环文本消息启动标志
extern bool g_loopTextRun;
// 循环文本消息计数器
extern Counter g_loopTextCounter;
// 循环文本消息接收用户列表
extern list<string> g_loopTextUserList;
// 循环文本消息发送间隔时间
extern int g_loopTextStepTime;

// 循环高表消息启动标志
extern bool g_loopEmotionRun;
// 循环高表消息计数器
extern Counter g_loopEmotionCounter;
// 循环高表消息接收用户列表
extern list<string> g_loopEmotionUserList;
// 循环高表消息发送间隔时间
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
	virtual bool HandleTheCmd(list<string>& cmdList,bool &exit) = 0;//公共处理cmd函数
};