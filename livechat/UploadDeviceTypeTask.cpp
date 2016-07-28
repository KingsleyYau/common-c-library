/*
 * author: Samson.Fan
 *   date: 2015-03-31
 *   file: UploadDeviceTypeTask.h
 *   desc: 上传设备类型Task实现类
 */


#include "UploadDeviceTypeTask.h"
#include "ILiveChatClient.h"
#include <amf/AmfParser.h>
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>

UploadDeviceTypeTask::UploadDeviceTypeTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_clientType = CLIENT_ANDROID;
}

UploadDeviceTypeTask::~UploadDeviceTypeTask(void)
{
}

// 初始化
bool UploadDeviceTypeTask::Init(ILiveChatClientListener* listener)
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
bool UploadDeviceTypeTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	// 本协议无返回
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool UploadDeviceTypeTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;

	// 构造json协议
	int clientType = (int)GetDeviceTypeWithClientType(m_clientType);
	Json::Value root(clientType);
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
TASK_PROTOCOL_TYPE UploadDeviceTypeTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int UploadDeviceTypeTask::GetCmdCode()
{
	return TCMD_UPLOADDEVTYPE;
}

// 设置seq
void UploadDeviceTypeTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int UploadDeviceTypeTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool UploadDeviceTypeTask::IsWaitToRespond()
{
	// 本协议无返回
	return false;
}

// 获取处理结果
void UploadDeviceTypeTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errmsg)
{
	errType = m_errType;
	errmsg = m_errMsg;
}

// 初始化参数
bool UploadDeviceTypeTask::InitParam(CLIENT_TYPE clientType)
{
	m_clientType = clientType;
	return true;
}

// 未完成任务的断线通知
void UploadDeviceTypeTask::OnDisconnect()
{
	// 不用回调
}
