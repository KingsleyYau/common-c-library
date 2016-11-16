/*
 * author: Alex shum
 *   date: 2016-09-08
 *   file: LCMagicIconDownloader.h
 *   desc: LiveChat小高级表情下载器
 */

#pragma once

#include <httpclient/HttpDownloader.h>
#include "LCMagicIconItem.h"

class LCMagicIconDownloaderCallback;
class LCMagicIconDownloader : private IHttpDownloaderCallback
{
public:
	// 小高级表情待下载的文件类型
	typedef enum {
		DEFAULT,			// 默认
		THUMB,				// 拇子图
		SOURCE		       // 原图
	} MagicIconDownloadType;

public:
	LCMagicIconDownloader();
	virtual ~LCMagicIconDownloader();

public:
	// 开始下载
	bool Start(const string& url
				, const string& filePath
				, MagicIconDownloadType downloadType
				, LCMagicIconItem* item
				, LCMagicIconDownloaderCallback* callback
				, const string& httpUser = ""
				, const string& httpPassword = "");
	// 停止下载
	void Stop();

// IHttpDownloaderCallback
private:
	// 下载成功
	virtual void onSuccess(HttpDownloader* loader);
	// 下载失败
	virtual void onFail(HttpDownloader* loader);
	// 下载进度更新
	virtual void onUpdate(HttpDownloader* loader);

private:
	HttpDownloader	m_downloader;	// 下载器
	LCMagicIconDownloaderCallback*	m_callback;		// 回调
	LCMagicIconItem*	m_magicIconItem;	// 小高级表情item
	MagicIconDownloadType	m_downloadType;		// 文件类型
};

class LCMagicIconDownloaderCallback
{
public:
	LCMagicIconDownloaderCallback() {};
	virtual ~LCMagicIconDownloaderCallback() {};
public:
	virtual void onSuccess(LCMagicIconDownloader* downloader, LCMagicIconDownloader::MagicIconDownloadType downloadType, LCMagicIconItem* item) = 0;
	virtual void onFail(LCMagicIconDownloader* downloader, LCMagicIconDownloader::MagicIconDownloadType downloadType, LCMagicIconItem* item) = 0;
};
