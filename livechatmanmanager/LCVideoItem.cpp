/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCVideoItem.h
 *   desc: LiveChat视频消息item
 */

#include "LCVideoItem.h"
#include <common/CommonFunc.h>
#include "RequestLiveChatDefine.h"

LCVideoItem::LCVideoItem()
{
	m_videoId = "";
	m_videoDesc = "";
	m_sendId = "";
	m_videoUrl = "";
	m_thumbPhotoFilePath = "";
	m_bigPhotoFilePath = "";
	m_videoFilePath = "";
	m_charge = false;
	m_isVideoFeeding = false;
	m_isThumbPhotoDownloading = false;
	m_isBigPhotoDownloading = false;
	m_isVideoDownloading = false;
	m_videoDownloadProgress = 0;
}

LCVideoItem::~LCVideoItem()
{

}

bool LCVideoItem::Init(
		const string& videoId
		, const string& sendId
		, const string& videoDesc
		, const string& bigPhotoFilePath
		, const string& thumbPhotoFilePath
		, const string& videoUrl
		, const string& videoFilePath
		, bool charge)
{
	m_videoId = videoId;
	m_sendId = sendId;
	m_videoDesc = videoDesc;
	m_charge = charge;
	m_videoUrl = videoUrl;

	if ( IsFileExist(bigPhotoFilePath) )
	{
		m_bigPhotoFilePath = bigPhotoFilePath;
	}

	if ( IsFileExist(thumbPhotoFilePath) )
	{
		m_thumbPhotoFilePath = thumbPhotoFilePath;
	}

	if ( IsFileExist(videoFilePath) )
	{
		m_videoFilePath = videoFilePath;
	}

	return true;
}

// 更新视频图片下载标志
void LCVideoItem::UpdatePhotoDownloadSign(VIDEO_PHOTO_TYPE type, bool isDownloading)
{
	switch (type)
	{
	case VPT_BIG: {
		m_isBigPhotoDownloading = isDownloading;
	}break;
	case VPT_DEFAULT: {
		m_isThumbPhotoDownloading = isDownloading;
	}break;
	}
}

// 更新视频图片路径
void LCVideoItem::UpdatePhotoPathWithType(const string& filePath, VIDEO_PHOTO_TYPE type)
{
	if ( !filePath.empty() )
	{
		if ( IsFileExist(filePath) )
		{
			switch (type)
			{
			case VPT_BIG: {
				m_bigPhotoFilePath = filePath;
			}break;
			case VPT_DEFAULT: {
				m_thumbPhotoFilePath = filePath;
			}break;
			}
		}
	}
}

