/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCVoiceItem.h
 *   desc: LiveChat语音消息item
 */

#include "LCVoiceItem.h"
#include <common/CommonFunc.h>
#include <manrequesthandler/RequestLiveChatDefine.h>
#include <common/CheckMemoryLeak.h>

LCVoiceItem::LCVoiceItem()
{
	m_voiceId = "";
	m_filePath = "";
	m_timeLength = 0;
	m_fileType = "";
	m_checkCode = "";
	m_charge = false;
	m_processing = false;
}

LCVoiceItem::~LCVoiceItem()
{

}

// 初始化
bool LCVoiceItem::Init(
					const string& voiceId
					, const string& filePath
					, int timeLength
					, const string& fileType
					, const string& checkCode
					, bool charge)
{
	m_voiceId = voiceId;
	m_timeLength = timeLength;
	m_fileType = fileType;
	m_checkCode = checkCode;
	m_charge = charge;
	m_processing = false;

	if ( IsFileExist(filePath) )
	{
		m_filePath = filePath;
	}

	return true;
}

