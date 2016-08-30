/*
 * author:Alex shem
 *   date:2016-8-24
 *   file:SendAutoInviteTask.h
 *   desc:����/�رշ����Զ�������Ϣ���񣨽�Ůʿ��
*/

#pragma once
#include "ITask.h"
#include <string>


class SendAutoInviteTask: public ITask
{
public:
	SendAutoInviteTask(void);
	virtual ~SendAutoInviteTask(void);

//ITask�ӿں���
public:
	// ��ʼ��
	virtual bool Init(ILiveChatClientListener* listener);
	// �����ѽ�������
	virtual bool Handle(const TransportProtocol* tp);
	// ��ȡ�����͵����ݣ����Ȼ�ȡdata���ȣ��磺GetSendData(NULL, 0, dataLen);
	virtual bool GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen);
	// ��ȡ���������ݵ�����
	virtual TASK_PROTOCOL_TYPE GetSendDataProtocolType();
    // ��ȡ�����
	virtual int GetCmdCode();
    // ����seq
	virtual void SetSeq(unsigned int seq);
	// ��ȡseq
	virtual unsigned int GetSeq();
    // �Ƿ���Ҫ�ȴ��ظ�����false���ͺ��ͷţ�delete�����������ͺ�ᱻ��������ظ��б����յ��ظ����ͷ�
	virtual bool IsWaitToRespond();
    // ��ȡ������
	virtual void GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg);
    // δ�������Ķ���֪ͨ
	virtual void OnDisconnect();

public:
	// ��ʼ������
	bool InitParam(bool isOpen);

private:
	ILiveChatClientListener*        m_listener;

	unsigned int     m_seq;         //seq
	LCC_ERR_TYPE     m_errType;     //���������صĴ�����
	string           m_errMsg;      //���������صĽ������

	bool             m_isOpen;     //�����Զ�������Ϣ��״̬


};