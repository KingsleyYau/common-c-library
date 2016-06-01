/*
 * author: Samson.Fan
 *   date: 2015-08-03
 *   file: RefreshInviteTemplateTask.cpp
 *   desc: 刷新邀请模板Task实现类
 */

#include "RefreshInviteTemplateTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include "CommonParsing.h"
#include <json/json/json.h>
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>


RefreshInviteTemplateTask::RefreshInviteTemplateTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";
}

RefreshInviteTemplateTask::~RefreshInviteTemplateTask(void)
{
}

// 初始化
bool RefreshInviteTemplateTask::Init(ILiveChatClientListener* listener)
{
	bool result = false;
	if (NULL != listener)
	{
		m_listener = listener;
		result = true;
	}
	return result;
}

// 处理已接收数据
bool RefreshInviteTemplateTask::Handle(const TransportProtocol* tp)
{
	// 没有返回
	return true;
}

// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RefreshInviteTemplateTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	// 没有请求参数
	dataLen = 0;
	return true;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RefreshInviteTemplateTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// 获取命令号
int RefreshInviteTemplateTask::GetCmdCode()
{
	return TCMD_REFRESHINVITETEMPLATE;
}

// 设置seq
void RefreshInviteTemplateTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RefreshInviteTemplateTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RefreshInviteTemplateTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RefreshInviteTemplateTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RefreshInviteTemplateTask::OnDisconnect()
{
	// 没有回调
}
