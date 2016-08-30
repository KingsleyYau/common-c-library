/*
 * author:Alex shem
 *   date:2016-8-23
 *   file:GetAutoInviteStatusTask.cpp
 *   desc:��ȡ�����Զ�������Ϣ״̬����Ůʿ��
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

//��ʼ��
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


//����������
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

		// ����ʧ��Э��
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
	// Э�����ʧ��
	if(!result){
		m_errType = LCC_ERR_PROTOCOLFAIL;
		m_errMsg = "";
	}
	// ֪ͨlistener
	if(NULL != m_listener){
		m_listener->OnGetAutoInviteMsgSwitchStatus(m_errType, m_errMsg, m_isOpenStatus);
	}
    return result;
}

// ��ȡ���͵���ݣ����Ȼ�ȡdata���ȣ��磺GetSendData(NULL, 0, dataLen);
bool GetAutoInviteStatusTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	// û�в���
	dataLen = 0;
	return true;
}

// ��ȡ������ݵ�����
TASK_PROTOCOL_TYPE GetAutoInviteStatusTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// ��ȡ�����
int GetAutoInviteStatusTask::GetCmdCode()
{
	return TCMD_GETAUTOINVITESTATUS;
}

// ����seq
void GetAutoInviteStatusTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// ��ȡseq
unsigned int GetAutoInviteStatusTask::GetSeq()
{
	return m_seq;
}

// �Ƿ���Ҫ�ȴ�ظ�����false���ͺ��ͷ�(delete��)�������ͺ�ᱻ�������ظ��б?�յ��ظ����ͷ�
bool GetAutoInviteStatusTask::IsWaitToRespond()
{
	return true;
}

// ��ȡ������
void GetAutoInviteStatusTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// δ�������Ķ���֪ͨ
void GetAutoInviteStatusTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnGetAutoInviteMsgSwitchStatus(LCC_ERR_CONNECTFAIL, "", false);
	}

}



