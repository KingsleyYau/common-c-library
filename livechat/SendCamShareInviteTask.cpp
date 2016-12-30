/*
 * author: Alex.shum
 *   date: 2016-10-27
 *   file: SendCamShareInviteTask.cpp
 *   desc: 发送CamShare邀请 Task实现类
 */

#include "SendCamShareInviteTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>
#include <common/KLog.h>

// CamShare对象Id
#define TARGETID_PARAM      "targetId"
// CamShare邀请信息
#define MSG_PARAM            "msg"
SendCamShareInviteTask::SendCamShareInviteTask(void)
{
	m_listener = NULL;
	
	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
	m_camShareMsg = "";
}

SendCamShareInviteTask::~SendCamShareInviteTask(void)
{
}

// 初始化
bool SendCamShareInviteTask::Init(ILiveChatClientListener* listener)
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
bool SendCamShareInviteTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	m_errType = LCC_ERR_SUCCESS;
	m_errMsg = "";

	bool success = false;

	AmfParser parser;
    amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if(!root.isnull()){
				// 解析成功协议
		if (root->type == DT_TRUE
			|| root->type == DT_FALSE)
		{
			success = (root->type == DT_TRUE);
			result = true;
		}

		// 解析失败协议
		int errType = 0;
		string errMsg = "";
		if (GetAMFProtocolError(root, errType, errMsg)) {
			m_errType = (LCC_ERR_TYPE)errType;
			m_errMsg = errMsg;

			// 解析成功
			result = true;
		}
	}

	FileLog("LiveChatClient", "SendCamShareInviteTask::Handle() result:%d, success:%d", result, success);

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnSendCamShareInvite(m_userId, m_errType, m_errMsg);
	}
	// 本命令无返回
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool SendCamShareInviteTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	// 构造json协议
	Json::Value root;
	root[TARGETID_PARAM] = m_userId;
	root[MSG_PARAM]      = m_camShareMsg;
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
TASK_PROTOCOL_TYPE SendCamShareInviteTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int SendCamShareInviteTask::GetCmdCode()
{
	return TCMD_SENDCAMSHAREINVITE;
}
	
// 设置seq
void SendCamShareInviteTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int SendCamShareInviteTask::GetSeq()
{
	return m_seq;
}
	
// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool SendCamShareInviteTask::IsWaitToRespond()
{
	return true;
}
	
// 获取处理结果
void SendCamShareInviteTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool SendCamShareInviteTask::InitParam(const string& userId, const string& camShareMsg)
{
	bool result = false;
	if (!userId.empty()) {
		m_userId = userId;
		m_camShareMsg = camShareMsg;
		result = true;
	}
	
	return result;
}

// 未完成任务的断线通知
void SendCamShareInviteTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnSendCamShareInvite(m_userId, LCC_ERR_CONNECTFAIL, "");
	}
}
