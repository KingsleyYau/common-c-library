/*
 * author: Alex.shum
 *   date: 2016-10-27
 *   file: ApplyCamShareTask.cpp
 *   desc: 男士发起CamShare并开始扣费（仅男士端) Task实现类
 */

#include "ApplyCamShareTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>
#include <common/KLog.h>

// CamShare对象Id
#define TARGETID_PARAM      "targetId"

ApplyCamShareTask::ApplyCamShareTask(void)
{
	m_listener = NULL;
	
	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
}

ApplyCamShareTask::~ApplyCamShareTask(void)
{
}

// 初始化
bool ApplyCamShareTask::Init(ILiveChatClientListener* listener)
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
bool ApplyCamShareTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	m_errType = LCC_ERR_SUCCESS;
	m_errMsg = "";

	bool success = false;
	string targetId = "";

	AmfParser parser;
    amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if(!root.isnull()){
				// 解析成功协议
		if (root->type == DT_STRING)
		{
			m_errType = LCC_ERR_SUCCESS;
			m_errMsg  = "";
			targetId = root->strValue;
			success = true;
			result = true;
		}
		if (!result){
			int errType = 0;
			string errMsg = "";
			if (GetAMFProtocolError(root, errType, errMsg)) {
				m_errType = (LCC_ERR_TYPE)errType;
				m_errMsg = errMsg;
				// 解析成功
				result = true;
			}
		}

	}

	FileLog("LiveChatClient", "ApplyCamShareTask::Handle() result:%d, success:%d", result, success);

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnApplyCamShare(m_userId, m_errType, m_errMsg, success, targetId);
	}
	// 本命令无返回	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool ApplyCamShareTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	// 构造json协议
	Json::Value root;
	//root[TARGETID_PARAM] = m_userId;
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
TASK_PROTOCOL_TYPE ApplyCamShareTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int ApplyCamShareTask::GetCmdCode()
{
	return TCMD_APPLYCAMSHARE;
}
	
// 设置seq
void ApplyCamShareTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int ApplyCamShareTask::GetSeq()
{
	return m_seq;
}
	
// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool ApplyCamShareTask::IsWaitToRespond()
{
	return true;
}
	
// 获取处理结果
void ApplyCamShareTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool ApplyCamShareTask::InitParam(const string& userId)
{
	bool result = false;
	if (!userId.empty()) {
		m_userId = userId;
		result = true;
	}
	
	return result;
}

// 未完成任务的断线通知
void ApplyCamShareTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnApplyCamShare(m_userId, LCC_ERR_CONNECTFAIL, "", false, "");
	}
}
