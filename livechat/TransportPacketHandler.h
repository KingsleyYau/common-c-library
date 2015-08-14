/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: TransportPacketHandler.h
 *   desc: 传输包处理实现类
 */

#pragma once

#include "ITransportPacketHandler.h"

class CTransportPacketHandler : public ITransportPacketHandler
{
public:
	CTransportPacketHandler(void);
	virtual ~CTransportPacketHandler(void);

public:
	// 组包
	virtual bool Packet(ITask* task, void* data, unsigned int dataSize, unsigned int& dataLen);
	// 解包
	virtual UNPACKET_RESULT_TYPE Unpacket(void* data, unsigned int dataLen, TransportProtocol** ppTp, unsigned int& useLen);

private:
	// 移位解密
	void ShiftRight(unsigned char *data, int length, unsigned char bit) ;
	// 解压
	bool Unzip(TransportProtocol* tp, TransportProtocol** ppTp);
	// 重建buffer
	bool RebuildBuffer();

private:
	unsigned char* m_buffer;
	unsigned int m_bufferLen;
};
