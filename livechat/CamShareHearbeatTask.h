/*
 * author: Alex.shum
 *   date: 2016-10-28
 *   file: CamShareHearbeatTask.h
 *   desc: CamShare心跳Task实现类
 */

#pragma once

#include "ITask.h"
#include <string>

using namespace std;

class  CamShareHearbeatTask : public ITask
{
public:
	 CamShareHearbeatTask(void);
	virtual ~ CamShareHearbeatTask(void);

// ITask接口函数
public:
	// 初始化
	virtual bool Init(ILiveChatClientListener* listener);
	// 处理已接收数据
	virtual bool Handle(const TransportProtocol* tp);
	// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
	virtual bool GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen);
	// 获取待发送数据的类型
	virtual TASK_PROTOCOL_TYPE GetSendDataProtocolType();
	// 获取命令号
	virtual int GetCmdCode();
	// 设置seq
	virtual void SetSeq(unsigned int seq);
	// 获取seq
	virtual unsigned int GetSeq();
	// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
	virtual bool IsWaitToRespond();
	// 获取处理结果
	virtual void GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg);
	// 未完成任务的断线通知
	virtual void OnDisconnect();

public:
	// 初始化参数
	bool InitParam(const string& userId, const string& inviteId);

private:
	ILiveChatClientListener*	m_listener;
	unsigned int	m_seq;

    // CamShare对象Id
	string  m_targetId;
	// CamShare会话Id
	string  m_inviteId;
};
