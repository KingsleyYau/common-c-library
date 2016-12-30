/*
 * author: Alex.shum
 *   date: 2016-10-27
 *   file: GetLadyCamStatus.cpp
 *   desc: 获取女士cam状态（仅男士端） Task实现类
 */

#include "GetLadyCamStatusTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>
#include <common/KLog.h>

// 女士ID
#define WOMANID_PARAM "womanId"

GetLadyCamStatusTask::GetLadyCamStatusTask(void)
{
	m_listener = NULL;
	
	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
}

GetLadyCamStatusTask::~GetLadyCamStatusTask(void)
{
}

// 初始化
bool GetLadyCamStatusTask::Init(ILiveChatClientListener* listener)
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
bool GetLadyCamStatusTask::Handle(const TransportProtocol* tp)
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

	FileLog("LiveChatClient", "GetLadyCamStatusTask::Handle() result:%d, success:%d", result, success);

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnGetLadyCamStatus(m_userId, m_errType, m_errMsg, success);
	}
	// 本命令无返回	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool GetLadyCamStatusTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	// 构造json协议
	Json::Value root;
	//root[WOMANID_PARAM] = m_userId;
	root = m_userId;
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
TASK_PROTOCOL_TYPE GetLadyCamStatusTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int GetLadyCamStatusTask::GetCmdCode()
{
	return TCMD_GETLADYCAMSTATUS;
}
	
// 设置seq
void GetLadyCamStatusTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int GetLadyCamStatusTask::GetSeq()
{
	return m_seq;
}
	
// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool GetLadyCamStatusTask::IsWaitToRespond()
{
	return true;
}
	
// 获取处理结果
void GetLadyCamStatusTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool GetLadyCamStatusTask::InitParam(const string& userId)
{
	bool result = false;
	if (!userId.empty()) {
		m_userId = userId;

		result = true;
	}
	return result;
}

// 未完成任务的断线通知
void GetLadyCamStatusTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnGetLadyCamStatus(m_userId, LCC_ERR_CONNECTFAIL, "", false);
	}
}
