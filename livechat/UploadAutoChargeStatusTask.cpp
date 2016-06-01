/*
 * author: Samson.Fan
 *   date: 2015-03-31
 *   file: UploadAutoChargeStatusTask.cpp
 *   desc: 上传弹出女士自动邀请消息Task实现类
 */

#include "UploadAutoChargeStatusTask.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>

UploadAutoChargeStatusTask::UploadAutoChargeStatusTask(void)
{
	m_listener = NULL;
	
	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_isCharge = false;
}

UploadAutoChargeStatusTask::~UploadAutoChargeStatusTask(void)
{
}

// 初始化
bool UploadAutoChargeStatusTask::Init(ILiveChatClientListener* listener)
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
bool UploadAutoChargeStatusTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	// 本命令无返回
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool UploadAutoChargeStatusTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	// 构造json协议
	Json::Value root;
	root = m_isCharge;
	Json::FastWriter writer;
	string json = writer.write(root);
	
	// 填入buffer
	if (json.length() < dataSize) {
		memcpy(data, json.c_str(), json.length());
		dataLen = json.length();

		result  = true;
	}
	return result;
}
	
// 获取待发送数据的类型
TASK_PROTOCOL_TYPE UploadAutoChargeStatusTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int UploadAutoChargeStatusTask::GetCmdCode()
{
	return TCMD_UPLOADAUTOCHARGE;
}
	
// 设置seq
void UploadAutoChargeStatusTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int UploadAutoChargeStatusTask::GetSeq()
{
	return m_seq;
}
	
// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool UploadAutoChargeStatusTask::IsWaitToRespond()
{
	return false;
}
	
// 获取处理结果
void UploadAutoChargeStatusTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool UploadAutoChargeStatusTask::InitParam(bool isCharge)
{
	m_isCharge = isCharge;
	return true;
}

// 未完成任务的断线通知
void UploadAutoChargeStatusTask::OnDisconnect()
{
	// 不用回调
}
