/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: TransportPacketHandler.cpp
 *   desc: 传输包处理实现类
 */

#include "TransportPacketHandler.h"
#include "ITask.h"
#include "zlib.h"
#include <common/KLog.h>

// 使用 ntohl 需要include跨平台socket头文件
#include "ISocketHandler.h"

CTransportPacketHandler::CTransportPacketHandler(void)
{
	m_buffer = NULL;
	m_bufferLen = 0;
}

CTransportPacketHandler::~CTransportPacketHandler(void)
{
	delete []m_buffer;
}

// 组包
bool CTransportPacketHandler::Packet(ITask* task, void* data, unsigned int dataSize, unsigned int& dataLen)
{
	//printf("CTransportPacketHandler::Packet() task:%p, data:%p, dataLen:%d\n", task, data, dataLen);

	FileLog("LiveChatClient", "CTransportPacketHandler::Packet() begin");

	bool result = false;

	if (task->GetSendDataProtocolType() == NOHEAD_PROTOCOL) {
		if (dataSize > sizeof(NoHeadTransportProtocol)) {
			NoHeadTransportProtocol* protocol = (NoHeadTransportProtocol*)data;

			// 获取数据
			unsigned int bodyLen = 0;
			result = task->GetSendData(protocol->data, dataSize - sizeof(NoHeadTransportProtocol), bodyLen);
			protocol->length = bodyLen;
			dataLen = protocol->GetAllDataLength();

			FileLog("LiveChatClient", "CTransportPacketHandler::Packet() NOHEAD_PROTOCOL, protocol->length:%d, dataLen:%d", protocol->length, dataLen);

			protocol->length = ntohl(protocol->length);
		}
	}
	else {
		if (dataSize > sizeof(TransportHeader)) {
			TransportProtocol* protocol = (TransportProtocol*)data;
			protocol->header.shiftKey = 0;
			protocol->header.remote = 0;
			protocol->header.request = 0;
			protocol->header.cmd = ntohl(task->GetCmdCode());
			protocol->header.seq = ntohl(task->GetSeq());
			protocol->header.zip = 0;	// 不压缩
			protocol->header.protocolType = (unsigned char)task->GetSendDataProtocolType();

			unsigned int bodyLen = 0;
			result = task->GetSendData(protocol->data, dataSize - sizeof(TransportHeader), bodyLen);
			dataLen = sizeof(TransportHeader) + bodyLen;

			protocol->header.length = dataLen - sizeof(protocol->header.length);
			protocol->header.length = ntohl(protocol->header.length);
		}
	}

	FileLog("LiveChatClient", "CTransportPacketHandler::Packet() end, result:%d", result);

	return result;
}
	
// 解包
UNPACKET_RESULT_TYPE CTransportPacketHandler::Unpacket(void* data, unsigned int dataLen, TransportProtocol** ppTp, unsigned int& useLen)
{
	UNPACKET_RESULT_TYPE result = UNPACKET_FAIL;
	useLen = 0;
	*ppTp = NULL;

	FileLog("LiveChatClient", "CTransportPacketHandler::Unpacket() begin");

	TransportProtocol* tp = (TransportProtocol*)data;
	tp->header.length = ntohl(tp->header.length);
	if (dataLen >= tp->header.length + sizeof(tp->header.length)) {
		ShiftRight((unsigned char*)data + sizeof(tp->header.length) + sizeof(tp->header.shiftKey), tp->header.length - sizeof(tp->header.shiftKey), tp->header.shiftKey);
		tp->header.cmd = ntohl(tp->header.cmd);
		tp->header.seq = ntohl(tp->header.seq);

		useLen = tp->header.length + sizeof(tp->header.length);
		if (tp->header.zip == 1
			&& tp->GetDataLength() > 0) 
		{
			// 需要解压
			result = Unzip(tp, ppTp) ? UNPACKET_SUCCESS : result;
		}
		else {
			*ppTp = tp;
			result = UNPACKET_SUCCESS;
		}
	}
	else {
		result = UNPACKET_MOREDATA;
	}

	FileLog("LiveChatClient", "CTransportPacketHandler::Unpacket() end, result:%d", result);

	return result;
}


// 移位解密
void CTransportPacketHandler::ShiftRight(unsigned char *data, int length, unsigned char bit) 
{
	if (length == 0) {
		return;
	}
	unsigned char b = bit % 8;
	if (b == 8) {
		return;
	}
	unsigned char mask = 0xff >> (8 - b);
	unsigned char last = data[length - 1];
	unsigned char *p;
	for (p = data; p < data + length; ++p) {
		unsigned char hight = (last & mask) << (8 - b);
		last = *p;		// & 0xff;
		*p = (last >> b) | hight;
	}
}

// 解压
bool CTransportPacketHandler::Unzip(TransportProtocol* tp, TransportProtocol** ppTp)
{
	// 初始化
	bool result = false;
	*ppTp = NULL;
	if (m_bufferLen == 0) {
		RebuildBuffer();
	}

	// 解压
	int unret = Z_OK;
	uLongf needBuffLen = 0;
	do {

		if (NULL != m_buffer && m_bufferLen > sizeof(tp->header)) {
			uLongf needBuffLen = m_bufferLen - sizeof(tp->header);
			uLong dataLen = tp->GetDataLength();
			unret = uncompress(m_buffer + sizeof(tp->header), &needBuffLen, tp->data, dataLen);
			if (Z_BUF_ERROR == unret) {
				// 缓冲不足
				RebuildBuffer();
				continue;
			}
			else {
				if (Z_OK == unret) {
					// 解压成功
					*ppTp = (TransportProtocol*)m_buffer;
					(*ppTp)->header = tp->header;
					(*ppTp)->SetDataLength(needBuffLen);

					result = true;
				}
				break;
			}
		}
		else {
			// 缓冲出错
			break;
		}
	} while (true);

	return result;
}

// 重建buffer
bool CTransportPacketHandler::RebuildBuffer()
{
	if (m_bufferLen == 0) {
		const unsigned int initBufferLen = 1024;
		m_buffer = new unsigned char[initBufferLen];
		if (NULL != m_buffer) {
			m_bufferLen = initBufferLen;
		}
	}
	else {
		delete []m_buffer;
		unsigned int reLen = m_bufferLen * 2;
		m_buffer = new unsigned char[reLen];
		if (NULL != m_buffer) {
			m_bufferLen = reLen;
			FileLog("LiveChatClient", "CTransportPacketHandler::RebuildBuffer() rebuild buffer, m_bufferLen:%d", m_bufferLen);
		}
	}

	return NULL != m_buffer;
}
