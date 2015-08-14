/*
 * author: Samson.Fan
 *   date: 2015-03-24
 *   file: ITask.h
 *   desc: Task（任务）接口类，一般情况下每个Task对应处理一个协议
 */

#pragma once

#include "ITransportPacketHandler.h"
#include "TaskDef.h"
#include "ILiveChatClient.h"
#include <list>

using namespace std;

class ILiveChatClientListener;
class ITask
{
public:
	static ITask* CreateTaskWithCmd(int cmd);

public:
	ITask(void) {};
	virtual ~ITask(void) {};

public:
	// 初始化
	virtual bool Init(ILiveChatClientListener* listener) = 0;
	// 处理已接收数据
	virtual bool Handle(const TransportProtocol* tp) = 0;
	// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
	virtual bool GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen) = 0;
	// 获取待发送数据的类型
	virtual TASK_PROTOCOL_TYPE GetSendDataProtocolType() = 0;
	// 获取命令号
	virtual int GetCmdCode() = 0;
	// 设置seq
	virtual void SetSeq(unsigned int seq) = 0;
	// 获取seq
	virtual unsigned int GetSeq() = 0;
	// 是否需要等待回复
	virtual bool IsWaitToRespond() = 0;
	// 获取处理结果
	virtual void GetHandleResult(LCC_ERR_TYPE& errType, string& errmsg) = 0;
	// 未完成任务的断线通知
	virtual void OnDisconnect() = 0;
};

// 定义任务列表
typedef list<ITask*> TaskList;
