/*
 * author: Samson.Fan
 *   date: 2015-03-30
 *   file: CheckVerTask.h
 *   desc: 检测版本Task实现类
 */


#include "CheckVerTask.h"
#include "ILiveChatClient.h"
#include <amf/AmfParser.h>
#include <json/json/json.h>
#include <common/KLog.h>

CheckVerTask::CheckVerTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_version = "";
}

CheckVerTask::~CheckVerTask(void)
{
}

// 初始化
bool CheckVerTask::Init(ILiveChatClientListener* listener)
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
bool CheckVerTask::Handle(const TransportProtocol* tp)
{
	bool result = false;

	FileLog("LiveChatClient", "CheckVerTask::Handle() tp.header.cmd:%d tp.header.length:%d, tp.dateLen:%d"
			, tp->header.cmd, tp->header.length, tp->GetDataLength());

	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()
		&& root->type == DT_FALSE || root->type == DT_TRUE) 
	{
		m_errType = root->boolValue ? LCC_ERR_SUCCESS : LCC_ERR_CHECKVERFAIL;
		m_errMsg = "";

		result = true;
	}

	// 协议解析失败
	if (!result) {
		m_errType = LCC_ERR_PROTOCOLFAIL;
		m_errMsg = "";
	}

	FileLog("LiveChatClient", "CheckVerTask::Handle() errType:%d", m_errType);

	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool CheckVerTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;

	// 构造json协议
	Json::Value root(m_version);
	Json::FastWriter writer;
	string json = writer.write(root);

	// 填入buffer
	if (json.length() < dataSize) {
		memcpy(data, json.c_str(), json.length());
		dataLen = json.length();

		result  = true;
	}

	FileLog("LiveChatClient", "CheckVerTask::GetSendData() dataSize:%d, json:%s, dataLen:%d", dataSize, json.c_str(), dataLen);

	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE CheckVerTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int CheckVerTask::GetCmdCode()
{
	return TCMD_CHECKVER;	
}

// 设置seq
void CheckVerTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int CheckVerTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool CheckVerTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void CheckVerTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errmsg)
{
	errType = m_errType;
	errmsg = m_errMsg;
}

// 初始化参数
bool CheckVerTask::InitParam(const string& version)
{
	if (!version.empty()) {
		m_version = version;
	}
	return !m_version.empty();
}

// 未完成任务的断线通知
void CheckVerTask::OnDisconnect()
{
	// 不需要通知上层
}
