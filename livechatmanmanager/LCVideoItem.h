/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCVideoItem.h
 *   desc: LiveChat视频消息item
 */

#pragma once

#include <string>
using namespace std;

class LCVideoItem
{
public:
	LCVideoItem();
	virtual ~LCVideoItem();

public:
	// 初始化
	bool Init(
		const string& videoId
		, const string& sendId
		, const string& videoDesc
		, const string& bigPhotoFilePath
		, const string& thumbPhotoFilePath
		, const string& videoUrl
		, const string& videoFilePath
		, bool charge);

	// 更新视频图片下载标志
	void UpdatePhotoDownloadSign(VIDEO_PHOTO_TYPE type, bool isDownloading);

	// 更新视频图片路径
	void UpdatePhotoPathWithType(const string& filePath, VIDEO_PHOTO_TYPE type);

public:
	string	m_videoId;				// 视频ID
	string	m_videoDesc;			// 视频描述
	string	m_sendId;				// 发送ID
	string	m_videoUrl;				// 视频URL
	string	m_thumbPhotoFilePath;	// 小图路径
	string	m_bigPhotoFilePath;		// 大图路径
	string	m_videoFilePath;		// 视频路径
	bool	m_charge;				// 是否已付费
	bool	m_isVideoFeeding;		// 是否正在付费
	bool	m_isThumbPhotoDownloading;	// 是否正在下载小图
	bool	m_isBigPhotoDownloading;	// 是否正在下载大图
	bool	m_isVideoDownloading;		// 是否正在下载视频
	int		m_videoDownloadProgress;	// 视频下载进度(0-100)
};
