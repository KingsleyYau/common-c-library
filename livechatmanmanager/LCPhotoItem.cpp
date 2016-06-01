/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoItem.h
 *   desc: LiveChat图片消息item
 */

#include "LCPhotoItem.h"
#include <manrequesthandler/RequestLiveChatController.h>
#include <common/CommonFunc.h>
#include <common/CheckMemoryLeak.h>

LCPhotoItem::LCPhotoItem()
{
	m_photoId = "";
	m_photoDesc = "";
	m_sendId = "";
	m_showFuzzyFilePath = "";
	m_thumbFuzzyFilePath = "";
	m_srcFilePath = "";
	m_showSrcFilePath = "";
	m_thumbSrcFilePath = "";
	m_charge = false;
}

LCPhotoItem::~LCPhotoItem()
{

}

// 初始化
bool LCPhotoItem::Init(const string& photoId				// 图片ID
						, const string& sendId				// 发送ID
						, const string& photoDesc			// 图片描述
						, const string& showFuzzyFilePath	// 用于显示的不清晰图路径
						, const string& thumbFuzzyFilePath	// 拇指不清晰图路径
						, const string& srcFilePath			// 原文件路径
						, const string& showSrcFilePath		// 用于显示的原图片路径
						, const string& thumbSrcFilePath	// 拇指原图路径
						, bool charge						// 是否已付费
						)
{
	m_photoId = photoId;
	m_sendId = sendId;
	m_photoDesc = photoDesc;
	m_charge = charge;

	if ( IsFileExist(showFuzzyFilePath) )
	{
		m_showFuzzyFilePath = showFuzzyFilePath;
	}

	if ( IsFileExist(thumbFuzzyFilePath) )
	{
		m_thumbFuzzyFilePath = thumbFuzzyFilePath;
	}

	if ( IsFileExist(srcFilePath) )
	{
		m_srcFilePath = srcFilePath;
	}

	if ( IsFileExist(showSrcFilePath) )
	{
		m_showSrcFilePath = showSrcFilePath;
	}

	if ( IsFileExist(thumbSrcFilePath) )
	{
		m_thumbSrcFilePath = thumbSrcFilePath;
	}

	return true;
}

// 设置图片处理状态
LCPhotoItem::ProcessStatus LCPhotoItem::GetProcessStatus(GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	ProcessStatus status = Unknow;
	if (modeType == GMT_CLEAR) {
		if (sizeType == GPT_LARGE
			|| sizeType == GPT_MIDDLE)
		{
			status = DownloadShowSrcPhoto;
		}
		else if (sizeType == GPT_ORIGINAL)
		{
			status = DownloadSrcPhoto;
		}
		else {
			status = DownloadThumbSrcPhoto;
		}
	}
	else if (modeType == GMT_FUZZY) {
		if (sizeType == GPT_LARGE
			|| sizeType == GPT_MIDDLE)
		{
			status = DownloadShowFuzzyPhoto;
		}
		else {
			status = DownloadThumbFuzzyPhoto;
		}
	}

	return status;
}

// 添加图片处理状态
void LCPhotoItem::AddProcessStatus(GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	ProcessStatus status = GetProcessStatus(modeType, sizeType);
	if (Unknow != status) {
		m_statusList.lock();
		m_statusList.push_back(status);
		m_statusList.unlock();
	}
}

// 移除图片处理状态
void LCPhotoItem::RemoveProcessStatus(GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	ProcessStatus status = GetProcessStatus(modeType, sizeType);
	if (Unknow != status) {
		m_statusList.lock();
		m_statusList.erase(status);
		m_statusList.unlock();
	}
}

// 判断是否处理状态
bool LCPhotoItem::IsProcessStatus(GETPHOTO_PHOTOMODE_TYPE modeType, GETPHOTO_PHOTOSIZE_TYPE sizeType)
{
	bool result = false;

	ProcessStatus status = GetProcessStatus(modeType, sizeType);
	m_statusList.lock();
	result = m_statusList.has(status);
	m_statusList.unlock();

	return result;
}

// 添加购买图片处理状态
void LCPhotoItem::AddFeeStatus()
{
	m_statusList.lock();
	if (!m_statusList.has(PhotoFee))
	{
		m_statusList.push_back(PhotoFee);
	}
	m_statusList.unlock();
}

// 移除购买图片处理状态
void LCPhotoItem::RemoveFeeStatus()
{
	m_statusList.lock();
	m_statusList.erase(PhotoFee);
	m_statusList.unlock();
}

// 判断是否付费状态
bool LCPhotoItem::IsFee()
{
	bool result = false;

	m_statusList.lock();
	result = m_statusList.has(PhotoFee);
	m_statusList.unlock();

	return result;
}
