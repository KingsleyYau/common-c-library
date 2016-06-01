/*
 * author: Samson.Fan
 *   date: 2015-08-03
 *   file: ReplyIdentifyCodeTask.cpp
 *   desc: 回复验证码Task实现类
 */

#include "ReplyIdentifyCodeTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include "CommonParsing.h"
#include <json/json/json.h>
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>


ReplyIdentifyCodeTask::ReplyIdentifyCodeTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_identifyCode = "";
}

ReplyIdentifyCodeTask::~ReplyIdentifyCodeTask(void)
{
}

// 初始化
bool ReplyIdentifyCodeTask::Init(ILiveChatClientListener* listener)
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
bool ReplyIdentifyCodeTask::Handle(const TransportProtocol* tp)
{
	bool result = false;

	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()) {
		if (root->type == DT_TRUE || root->type == DT_FALSE) {
			m_errType = (root->type==DT_TRUE ? LCC_ERR_SUCCESS : LCC_ERR_FAIL);
			m_errMsg = "";
			result = true;
		}

		// 解析失败协议
		if (!result) {
			int errType = 0;
			string errMsg = "";
			if (GetAMFProtocolError(root, errType, errMsg)) {
				m_errType = (LCC_ERR_TYPE)errType;
				m_errMsg = errMsg;
				result = true;
			}
		}
	}

	// 协议解析失败
	if (!result) {
		m_errType = LCC_ERR_PROTOCOLFAIL;
		m_errMsg = "";
	}

	FileLog("LiveChatClient", "ReplyIdentifyCodeTask::Handle() result:%d, m_errType:%d", result, m_errType);

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnReplyIdentifyCode(m_errType, m_errMsg);
	}

	return result;
}

// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool ReplyIdentifyCodeTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
    bool result = false;

	// 构造json协议
	Json::Value root(m_identifyCode);
	Json::FastWriter writer;
	string json = writer.write(root);

	// 填入buffer
	if (json.length() < dataSize) {
		memcpy(data, json.c_str(), json.length());
		dataLen = json.length();

		result = true;
	}
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE ReplyIdentifyCodeTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// 获取命令号
int ReplyIdentifyCodeTask::GetCmdCode()
{
	return TCMD_REPLYIDENTIFYCODE;
}

// 设置seq
void ReplyIdentifyCodeTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int ReplyIdentifyCodeTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool ReplyIdentifyCodeTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void ReplyIdentifyCodeTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void ReplyIdentifyCodeTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnReplyIdentifyCode(LCC_ERR_CONNECTFAIL, "");
	}
}

// 初始化变量
bool ReplyIdentifyCodeTask::InitParam(const string& identifyCode)
{
    bool result = false;
    if (!identifyCode.empty()) {
        m_identifyCode = identifyCode;
        result = true;
    }
    return result;
}
