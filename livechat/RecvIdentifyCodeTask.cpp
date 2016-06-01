/*
 * author: Samson.Fan
 *   date: 2015-08-04
 *   file: RecvIdentifyCodeTask.cpp
 *   desc: 接收验证码Task实现类
 */

#include "RecvIdentifyCodeTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>

RecvIdentifyCodeTask::RecvIdentifyCodeTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";
}

RecvIdentifyCodeTask::~RecvIdentifyCodeTask(void)
{
}

// 初始化
bool RecvIdentifyCodeTask::Init(ILiveChatClientListener* listener)
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
bool RecvIdentifyCodeTask::Handle(const TransportProtocol* tp)
{
    bool result = false;

    const unsigned char* data = NULL;
    long dataLen = 0;

    AmfParser parser;
    amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
    if (!root.isnull()
        && root->type == DT_BYTEARRAY)
    {
        data = root->bytearrayValue;
        dataLen = root->bytearrayLen;
        result = true;
    }

	// 通知listener
	if (NULL != m_listener
		&& result)
	{
		m_listener->OnRecvIdentifyCode(data, dataLen);
	}

	return result;
}

// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvIdentifyCodeTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvIdentifyCodeTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// 获取命令号
int RecvIdentifyCodeTask::GetCmdCode()
{
	return TCMD_RECVIDENTIFYCODE;
}

// 设置seq
void RecvIdentifyCodeTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvIdentifyCodeTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvIdentifyCodeTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvIdentifyCodeTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvIdentifyCodeTask::OnDisconnect()
{
	// 不需要回调
}
