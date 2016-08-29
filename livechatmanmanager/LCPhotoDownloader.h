/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoDownloader.h
 *   desc: LiveChat图片下载器
 */

#pragma once

#include <string>
#include <common/map_lock.h>
#include <manrequesthandler/RequestLiveChatController.h>
using namespace std;

class HttpRequestManager;
class LCMessageItem;
class LCPhotoManager;
class LCPhotoDownloaderCallback;
class LCPhotoDownloader
{
public:
	LCPhotoDownloader();
	virtual ~LCPhotoDownloader();

public:
	bool Init(HttpRequestManager* requestMgr, RequestLiveChatController* requestController, LCPhotoManager* photoMgr);
	bool StartGetPhoto(
			LCPhotoDownloaderCallback* callback
			, LCMessageItem* item
			, const string& userId
			, const string& sid
			, GETPHOTO_PHOTOSIZE_TYPE sizeType
			, GETPHOTO_PHOTOMODE_TYPE modeType);
	bool Stop();
	long GetRequestId() const;

	// 回调函数
public:
	void OnGetPhoto(long requestId, bool success, const string& errnum, const string& errmsg, const string& filePath);
    
private:
    bool IsStart();
    bool IsFinish();

private:
	LCPhotoManager*	m_photoMgr;
	HttpRequestManager*	m_requestMgr;
	RequestLiveChatController*	m_requestController;
	LCPhotoDownloaderCallback*	m_callback;
	long	m_requestId;
	LCMessageItem*	m_item;
	GETPHOTO_PHOTOSIZE_TYPE	m_sizeType;
	GETPHOTO_PHOTOMODE_TYPE m_modeType;
    bool    m_isStart;
};

class LCPhotoDownloaderCallback
{
public:
	LCPhotoDownloaderCallback() {};
	virtual ~LCPhotoDownloaderCallback() {};
public:
	virtual void onSuccess(LCPhotoDownloader* downloader, GETPHOTO_PHOTOSIZE_TYPE sizeType, LCMessageItem* item) = 0;
	virtual void onFail(LCPhotoDownloader* downloader, GETPHOTO_PHOTOSIZE_TYPE sizeType, const string& errnum, const string& errmsg, LCMessageItem* item) = 0;
};
