/*
 * author: Alex shem
 *   date: 2016-8-23
 *   file: GetAutoInviteStatusTask.cpp
 *   desc: 获取发送自动邀请消息状态（仅女士）
*/

#include "GetAutoInviteStatusTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>


GetAutoInviteStatusTask::GetAutoInviteStatusTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg  = "";

	m_isOpenStatus = false;
}

GetAutoInviteStatusTask::~GetAutoInviteStatusTask(void)
{
}

//初始化
bool GetAutoInviteStatusTask::Init(ILiveChatClientListener* listener)
{
	bool result = false;
	if(NULL != listener)
	{
		m_listener = listener;
		result = true;
	}
	return result;
}


//处理接收数据
bool GetAutoInviteStatusTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if(!root.isnull()){
		if(root->type == DT_FALSE || root->type == DT_TRUE)
		{
			m_errType = LCC_ERR_SUCCESS;
			m_errMsg  = "";
			m_isOpenStatus = root->boolValue;
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
	if(!result){
		m_errType = LCC_ERR_PROTOCOLFAIL;
		m_errMsg = "";
	}
	//通知listener
	if(NULL != m_listener){
		m_listener->OnGetAutoInviteMsgSwitchStatus(m_errType, m_errMsg, m_isOpenStatus);
	}
    return result;
}

// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool GetAutoInviteStatusTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	// 没有参数
	dataLen = 0;
	return true;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE GetAutoInviteStatusTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// 获取命令号
int GetAutoInviteStatusTask::GetCmdCode()
{
	return TCMD_GETAUTOINVITESTATUS;
}

// 设置seq
void GetAutoInviteStatusTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

//  获取seq
unsigned int GetAutoInviteStatusTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool GetAutoInviteStatusTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void GetAutoInviteStatusTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void GetAutoInviteStatusTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnGetAutoInviteMsgSwitchStatus(LCC_ERR_CONNECTFAIL, "", false);
	}

}



