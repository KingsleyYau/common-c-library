/*
 * author:Alex shem
 *   date:2016-8-24
 *   file:SendAutoInviteTask.cpp
 *   desc:����/�رշ����Զ�������Ϣ���񣨽�Ůʿ��
*/

#include "SendAutoInviteTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>


SendAutoInviteTask::SendAutoInviteTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg  = "";

	m_isOpen = false;
}

SendAutoInviteTask::~SendAutoInviteTask(void)
{
}

//��ʼ��
bool SendAutoInviteTask::Init(ILiveChatClientListener* listener)
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
bool SendAutoInviteTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if(!root.isnull()){
		if(root->type == DT_FALSE || root->type == DT_TRUE)
		{
			m_errType = LCC_ERR_SUCCESS;
			m_errMsg  = "";
			m_isOpen = root->boolValue;
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
		m_listener->OnSwitchAutoInviteMsg(m_errType, m_errMsg, m_isOpen);
	}
    return result;
}

// ��ȡ���͵���ݣ����Ȼ�ȡdata���ȣ��磺GetSendData(NULL, 0, dataLen);
bool SendAutoInviteTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	// ����jsonЭ��
	Json::Value root;
	root = m_isOpen;
	Json::FastWriter writer;
	string json = writer.write(root);
	
	// ����buffer
	if (json.length() < dataSize) {
		memcpy(data, json.c_str(), json.length());
		dataLen = json.length();

		result  = true;
	}
	return result;
}

// ��ȡ������ݵ�����
TASK_PROTOCOL_TYPE SendAutoInviteTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// ��ȡ�����
int SendAutoInviteTask::GetCmdCode()
{
	return TCMD_SENDAUTOINVITE;
}

// ����seq
void SendAutoInviteTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// ��ȡseq
unsigned int SendAutoInviteTask::GetSeq()
{
	return m_seq;
}

// �Ƿ���Ҫ�ȴ�ظ�����false���ͺ��ͷ�(delete��)�������ͺ�ᱻ�������ظ��б?�յ��ظ����ͷ�
bool SendAutoInviteTask::IsWaitToRespond()
{
	return true;
}

// ��ȡ������
void SendAutoInviteTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// ��ʼ������
bool SendAutoInviteTask::InitParam(bool isOpen)
{
	m_isOpen = isOpen;
	return true;
}

// δ�������Ķ���֪ͨ
void SendAutoInviteTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnSwitchAutoInviteMsg(LCC_ERR_CONNECTFAIL, "", false);
	}
}



