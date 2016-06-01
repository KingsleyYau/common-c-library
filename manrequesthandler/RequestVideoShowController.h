/*
 * RequestVideoShowController.h
 *
 *  Created on: 2015-3-6
 *      Author: Samson.Fan
 */

#ifndef REQUESTVIDEOSHOWCONTROLLER_H_
#define REQUESTVIDEOSHOWCONTROLLER_H_

#include "RequestBaseController.h"
#include "item/VideoShow.h"

typedef void (*OnRequestVSVideoList)(long requestId, bool success, const string& errnum, const string& errmsg, int pageIndex, int pageSize, int dataCount, const VSVideoList& vsList);
typedef void (*OnRequestVSVideoDetail)(long requestId, bool success, const string& errnum, const string& errmsg, const VSVideoDetailList& list);
typedef void (*OnRequestVSPlayVideo)(long requestId, bool success, const string& errnum, const string& errmsg, int memberType, const VSPlayVideoItem& item);
typedef void (*OnRequestVSWatchedVideoList)(long requestId, bool success, const string& errnum, const string& errmsg, int pageIndex, int pageSize, int dataCount, const VSWatchedVideoList& vsList);
typedef void (*OnRequestVSSaveVideo)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef void (*OnRequestVSRemoveVideo)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef void (*OnRequestVSSavedVideoList)(long requestId, bool success, const string& errnum, const string& errmsg, int pageIndex, int pageSize, int dataCount, const VSSavedVideoList& vsList);
typedef struct _tagRequestVideoShowControllerCallback {
	OnRequestVSVideoList onRequestVSVideoList;
	OnRequestVSVideoDetail onRequestVSVideoDetail;
	OnRequestVSPlayVideo onRequestVSPlayVideo;
	OnRequestVSWatchedVideoList onRequestVSWatchedVideoList;
	OnRequestVSSaveVideo onRequestVSSaveVideo;
	OnRequestVSRemoveVideo onRequestVSRemoveVideo;
	OnRequestVSSavedVideoList onRequestVSSavedVideoList;
} RequestVideoShowControllerCallback;


class RequestVideoShowController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestVideoShowController(HttpRequestManager* pHttpRequestManager, const RequestVideoShowControllerCallback& callback);
	virtual ~RequestVideoShowController();

public:
	long VideoList(int pageIndex, int pageSize, int age1, int age2, int orderBy);
	long VideoDetail(const string& womanid);
	long PlayVideo(const string& womanid, const string& videoid);
	long WatchedVideoList(int pageIndex, int pageSize);
	long SaveVideo(const string& videoid);
	long RemoveVideo(const string& videoid);
	long SavedVideoList(int pageIndex, int pageSize);

private:
	void VideoListCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void VideoDetailCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void PlayVideoCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void WatchedVideoListCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void SaveVideoCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void RemoveVideoCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void SavedVideoListCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);

protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	RequestVideoShowControllerCallback m_Callback;
};

#endif /* REQUESTVIDEOSHOWCONTROLLER_H_ */
