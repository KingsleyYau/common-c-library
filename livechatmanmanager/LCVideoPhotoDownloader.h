/*
 * author: Samson.Fan
 *   date: 2016-01-28
 *   file: LCVideoPhotoDownloader.h
 *   desc: LiveChat视频图片下载器
 */

#pragma once

#include <string>
#include <manrequesthandler/RequestLiveChatController.h>
using namespace std;

class LCVideoManager;
class HttpRequestManager;
class RequestLiveChatController;
class LCVideoPhotoDownloaderCallback;
class LCVideoPhotoDownloader
{
public:
	LCVideoPhotoDownloader();
	virtual ~LCVideoPhotoDownloader();

public:
	bool Init(HttpRequestManager* requestMgr, RequestLiveChatController* requestController, LCVideoManager* videoMgr);
	bool StartDownload(
			LCVideoPhotoDownloaderCallback* callback
			, const string& userId
			, const string& sid
			, const string& womanId
			, const string& videoId
			, const string& inviteId
			, VIDEO_PHOTO_TYPE type
			, const string& filePath);
	bool Stop();
	long GetRequestId() const;
	
	// 获取请求参数
	string GetVideoId() const;
	string GetWomanId() const;
	string GetInviteId() const;
	VIDEO_PHOTO_TYPE GetVideoPhotoType() const;
	string GetFilePath() const;

	// 回调函数
public:
	void OnGetVideoPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath);

private:
	LCVideoManager*	m_videoMgr;
	HttpRequestManager*	m_requestMgr;
	RequestLiveChatController*	m_requestController;
	LCVideoPhotoDownloaderCallback*	m_callback;
	
	long	m_requestId;

	string	m_womanId;
	string	m_videoId;
	string	m_inviteId;
	VIDEO_PHOTO_TYPE	m_type;
	string	m_filePath;
};

class LCVideoPhotoDownloaderCallback
{
public:
	LCVideoPhotoDownloaderCallback() {};
	virtual ~LCVideoPhotoDownloaderCallback() {};
public:
	virtual void onFinish(LCVideoPhotoDownloader* downloader, bool success, const string& errnum, const string& errmsg) = 0;
};
