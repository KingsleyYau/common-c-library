/*
 * author: Samson.Fan
 *   date: 2015-06-16
 *   file: UploadVerTask.cpp
 *   desc: 上传客户端版本号Task实现类
 */

#include "UploadVerTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>

UploadVerTask::UploadVerTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_ver = "";
}

UploadVerTask::~UploadVerTask(void)
{
}

// 初始化
bool UploadVerTask::Init(ILiveChatClientListener* listener)
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
bool UploadVerTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	// 本命令无返回
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool UploadVerTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;

	// 构造json协议
	Json::Value root(m_ver);
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
TASK_PROTOCOL_TYPE UploadVerTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int UploadVerTask::GetCmdCode()
{
	return TCMD_UPLOADVER;
}

// 设置seq
void UploadVerTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int UploadVerTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool UploadVerTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void UploadVerTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void UploadVerTask::OnDisconnect()
{
	// 不用回调
}

// 初始化参数
bool UploadVerTask::InitParam(const string& ver)
{
	bool result = false;
	if (!ver.empty())
	{
		m_ver = ver;
		result = true;
	}
	return result;
}

