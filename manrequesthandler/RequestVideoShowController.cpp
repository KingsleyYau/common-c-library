/*
 * RequestVideoShowController.cpp
 *
 *  Created on: 2015-3-6
 *      Author: Samson.Fan
 */

#include "RequestVideoShowController.h"
#include "RequestDefine.h"
#include "RequestVideoShowDefine.h"
#include "../common/CommonFunc.h"

RequestVideoShowController::RequestVideoShowController(HttpRequestManager *pHttpRequestManager, const RequestVideoShowControllerCallback& callback)
{
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(pHttpRequestManager);
	m_Callback = callback;
}

RequestVideoShowController::~RequestVideoShowController()
{
	// TODO Auto-generated destructor stub
}

/* IHttpRequestManagerCallback */
void RequestVideoShowController::onSuccess(long requestId, string url, const char* buf, int size)
{
	FileLog("httprequest", "RequestVideoShowController::onSuccess( url : %s, buf( size : %d ) )", url.c_str(), size);
	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestVideoShowController::onSuccess(), buf: %s", buf);
	}

	if( url.compare(VS_VIDEOLIST_PATH) == 0 ) {
		VideoListCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(VS_VIDEODETAIL_PATH) == 0 ) {
		VideoDetailCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(VS_PLAYVIDEO_PATH) == 0 ) {
		PlayVideoCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(VS_WATCHEDVIDEO_PATH) == 0 ) {
		WatchedVideoListCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(VS_SAVEVIDEO_PATH) == 0 ) {
		SaveVideoCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(VS_REMOVEVIDEO_PATH) == 0 ) {
		RemoveVideoCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(VS_SAVEDVIDEO_PATH) == 0 ) {
		SavedVideoListCallbackHandle(requestId, url, true, buf, size);
	}
	FileLog("httprequest", "RequestVideoShowController::onSuccess() end, url:%s", url.c_str());
}

void RequestVideoShowController::onFail(long requestId, string url)
{
	FileLog("httprequest", "RequestVideoShowController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(VS_VIDEOLIST_PATH) == 0 ) {
		VideoListCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(VS_VIDEODETAIL_PATH) == 0 ) {
		VideoDetailCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(VS_PLAYVIDEO_PATH) == 0 ) {
		PlayVideoCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(VS_WATCHEDVIDEO_PATH) == 0 ) {
		WatchedVideoListCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(VS_SAVEVIDEO_PATH) == 0 ) {
		SaveVideoCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(VS_REMOVEVIDEO_PATH) == 0 ) {
		RemoveVideoCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(VS_SAVEDVIDEO_PATH) == 0 ) {
		SavedVideoListCallbackHandle(requestId, url, false, NULL, 0);
	}
	FileLog("httprequest", "RequestVideoShowController::onFail() end, url:%s", url.c_str());
}

// ----------------------- VideoList -----------------------
long RequestVideoShowController::VideoList(int pageIndex, int pageSize, int age1, int age2, int orderBy)
{
	char temp[16];
	HttpEntiy entiy;

	// pageIndex
	sprintf(temp, "%d", pageIndex);
	entiy.AddContent(COMMON_PAGE_INDEX, temp);
	// pageSize
	sprintf(temp, "%d", pageSize);
	entiy.AddContent(COMMON_PAGE_SIZE, temp);
	// age1
	sprintf(temp, "%d", age1);
	entiy.AddContent(VS_REQUEST_AGE1, temp);
	// age2
	sprintf(temp, "%d", age2);
	entiy.AddContent(VS_REQUEST_AGE2, temp);
	// orderBy
	string strOrderBy("");
	if (orderBy < _countof(VS_ORDERBY_TYPE)) {
		sprintf(temp, "%d", VS_ORDERBY_TYPE[orderBy]);
		strOrderBy = temp;
		entiy.AddContent(VS_REQUEST_ORDERBY, strOrderBy.c_str());
	}

	string url = VS_VIDEOLIST_PATH;
	FileLog("httprequest", "RequestVideoShowController::VideoList"
			"(url:%s, pageIndex:%d, pageSize:%d, age1:%d, age2:%d, orderBy:%s)",
			url.c_str(), pageIndex, pageSize, age1, age2, strOrderBy.c_str());

	return StartRequest(url, entiy, this);
}

void RequestVideoShowController::VideoListCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	VSVideoList list;
	int pageIndex = 0, pageSize = 0, dataCount = 0;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			if (dataJson.isObject()) {
				// 解析pageIndex、pageCount、dataCount
				Json::Value pageIndexJson = dataJson[COMMON_PAGE_INDEX];
				if (pageIndexJson.isIntegral()) {
					pageIndex = pageIndexJson.asInt();
				}

				Json::Value pageSizeJson = dataJson[COMMON_PAGE_SIZE];
				if (pageSizeJson.isIntegral()) {
					pageSize = pageSizeJson.asInt();
				}

				Json::Value dataCountJson = dataJson[COMMON_DATA_COUNT];
				if (dataCountJson.isIntegral()) {
					dataCount = dataCountJson.asInt();
				}

				// 解析列表
				Json::Value dataListJson = dataJson[COMMON_DATA_LIST];
				bFlag = dataListJson.isArray();
				if(bFlag) {
					for(int i = 0; i < dataListJson.size(); i++) {
						VSVideoListItem item;
						if (item.Parsing(dataListJson.get(i, Json::Value::null))) {
							list.push_back(item);
						}
					}
				}
			}

			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestVideoShowController::VideoListCallbackHandle() parsing fail:"
						"(url:%s, size:%d, buf:%s)",
						url.c_str(), size, buf);
			}
		}
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestVSVideoList != NULL ) {
		m_Callback.onRequestVSVideoList(requestId, bFlag, errnum, errmsg, pageIndex, pageSize, dataCount, list);
	}
}

// ----------------------- VideoDetail -----------------------
long RequestVideoShowController::VideoDetail(const string& womanid)
{
	HttpEntiy entiy;
	entiy.AddContent(VS_REQUEST_WOMANID, womanid.c_str());

	string url = VS_VIDEODETAIL_PATH;
	FileLog("httprequest", "RequestVideoShowController::VideoDetail"
			"(url:%s, womanid:%s)",
			url.c_str(), womanid.c_str());

	return StartRequest(url, entiy, this);
}

void RequestVideoShowController::VideoDetailCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	VSVideoDetailList list;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			// success
			// 解析列表
			bFlag = dataJson.isArray();
			if(bFlag) {
				for(int i = 0; i < dataJson.size(); i++) {
					VSVideoDetailItem item;
					if (item.Parsing(dataJson.get(i, Json::Value::null))) {
						list.push_back(item);
					}
				}
			}

			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestVideoShowController::VideoDetailCallbackHandle() parsing fail:"
							"(url:%s, size:%d, buf:%s)",
							url.c_str(), size, buf);
			}
		}
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestVSVideoDetail != NULL ) {
		m_Callback.onRequestVSVideoDetail(requestId, bFlag, errnum, errmsg, list);
	}
}

// ----------------------- PlayVideo -----------------------
long RequestVideoShowController::PlayVideo(const string& womanid, const string& videoid)
{
	HttpEntiy entiy;
	entiy.AddContent(VS_REQUEST_WOMANID, womanid.c_str());
	entiy.AddContent(VS_REQUEST_VIDEOID, videoid.c_str());

	string url = VS_PLAYVIDEO_PATH;
	FileLog("httprequest", "RequestVideoShowController::PlayVideo"
			"(url:%s, womanid:%s, videoid:%s)",
			url.c_str(), womanid.c_str(), videoid.c_str());

	return StartRequest(url, entiy, this);
}

void RequestVideoShowController::PlayVideoCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	VSPlayVideoItem item;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			// success
			bFlag = item.Parsing(dataJson);
			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestVideoShowController::PlayVideoCallbackHandle() parsing fail:"
							"(url:%s, size:%d, buf:%s)",
							url.c_str(), size, buf);
			}
		}
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestVSPlayVideo != NULL ) {
		m_Callback.onRequestVSPlayVideo(requestId, bFlag, errnum, errmsg, item);
	}
}

// ----------------------- WatchedVideoList -----------------------
long RequestVideoShowController::WatchedVideoList(int pageIndex, int pageSize)
{
	char temp[16];
	HttpEntiy entiy;

	// pageIndex
	sprintf(temp, "%d", pageIndex);
	entiy.AddContent(COMMON_PAGE_INDEX, temp);
	// pageSize
	sprintf(temp, "%d", pageSize);
	entiy.AddContent(COMMON_PAGE_SIZE, temp);

	string url = VS_WATCHEDVIDEO_PATH;
	FileLog("httprequest", "RequestVideoShowController::WatchedVideoList"
			"(url:%s, pageIndex:%d, pageSize:%d)",
			url.c_str(), pageIndex, pageSize);

	return StartRequest(url, entiy, this);
}

void RequestVideoShowController::WatchedVideoListCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	VSWatchedVideoList list;
	int pageIndex = 0, pageSize = 0, dataCount = 0;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			if (dataJson.isObject()) {
				// 解析pageIndex、pageCount、dataCount
				Json::Value pageIndexJson = dataJson[COMMON_PAGE_INDEX];
				if (pageIndexJson.isIntegral()) {
					pageIndex = pageIndexJson.asInt();
				}

				Json::Value pageSizeJson = dataJson[COMMON_PAGE_SIZE];
				if (pageSizeJson.isIntegral()) {
					pageSize = pageSizeJson.asInt();
				}

				Json::Value dataCountJson = dataJson[COMMON_DATA_COUNT];
				if (dataCountJson.isIntegral()) {
					dataCount = dataCountJson.asInt();
				}

				// 解析列表
				Json::Value dataListJson = dataJson[COMMON_DATA_LIST];
				bFlag = dataListJson.isArray();
				if(bFlag) {
					for(int i = 0; i < dataListJson.size(); i++) {
						VSWatchedVideoListItem item;
						if (item.Parsing(dataListJson.get(i, Json::Value::null))) {
							list.push_back(item);
						}
					}
				}
			}

			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestVideoShowController::WatchedVideoListCallbackHandle() parsing fail:"
						"(url:%s, size:%d, buf:%s)",
						url.c_str(), size, buf);
			}
		}
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestVSWatchedVideoList != NULL ) {
		m_Callback.onRequestVSWatchedVideoList(requestId, bFlag, errnum, errmsg, pageIndex, pageSize, dataCount, list);
	}
}

// ----------------------- SaveVideo -----------------------
long RequestVideoShowController::SaveVideo(const string& videoid)
{
	HttpEntiy entiy;
	entiy.AddContent(VS_REQUEST_VIDEOID, videoid.c_str());

	string url = VS_SAVEVIDEO_PATH;
	FileLog("httprequest", "RequestVideoShowController::SaveVideo"
			"(url:%s, videoid:%s)",
			url.c_str(), videoid.c_str());

	return StartRequest(url, entiy, this);
}

void RequestVideoShowController::SaveVideoCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		bFlag = HandleResult(buf, size, errnum, errmsg, &dataJson);
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestVSSaveVideo != NULL ) {
		m_Callback.onRequestVSSaveVideo(requestId, bFlag, errnum, errmsg);
	}
}

// ----------------------- RemoveVideo -----------------------
long RequestVideoShowController::RemoveVideo(const string& videoid)
{
	HttpEntiy entiy;
	entiy.AddContent(VS_REQUEST_VIDEOID, videoid.c_str());

	string url = VS_REMOVEVIDEO_PATH;
	FileLog("httprequest", "RequestVideoShowController::RemoveVideo"
			"(url:%s, videoid:%s)",
			url.c_str(), videoid.c_str());

	return StartRequest(url, entiy, this);
}

void RequestVideoShowController::RemoveVideoCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		bFlag = HandleResult(buf, size, errnum, errmsg, &dataJson);
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestVSRemoveVideo != NULL ) {
		m_Callback.onRequestVSRemoveVideo(requestId, bFlag, errnum, errmsg);
	}
}

// ----------------------- SavedVideoList -----------------------
long RequestVideoShowController::SavedVideoList(int pageIndex, int pageSize)
{
	char temp[16];
	HttpEntiy entiy;

	// pageIndex
	sprintf(temp, "%d", pageIndex);
	entiy.AddContent(COMMON_PAGE_INDEX, temp);
	// pageSize
	sprintf(temp, "%d", pageSize);
	entiy.AddContent(COMMON_PAGE_SIZE, temp);

	string url = VS_SAVEDVIDEO_PATH;
	FileLog("httprequest", "RequestVideoShowController::SavedVideoList"
			"(url:%s, pageIndex:%d, pageSize:%d)",
			url.c_str(), pageIndex, pageSize);

	return StartRequest(url, entiy, this);
}

void RequestVideoShowController::SavedVideoListCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	VSSavedVideoList list;
	int pageIndex = 0, pageSize = 0, dataCount = 0;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			if (dataJson.isObject()) {
				// 解析pageIndex、pageCount、dataCount
				Json::Value pageIndexJson = dataJson[COMMON_PAGE_INDEX];
				if (pageIndexJson.isIntegral()) {
					pageIndex = pageIndexJson.asInt();
				}

				Json::Value pageSizeJson = dataJson[COMMON_PAGE_SIZE];
				if (pageSizeJson.isIntegral()) {
					pageSize = pageSizeJson.asInt();
				}

				Json::Value dataCountJson = dataJson[COMMON_DATA_COUNT];
				if (dataCountJson.isIntegral()) {
					dataCount = dataCountJson.asInt();
				}

				// 解析列表
				Json::Value dataListJson = dataJson[COMMON_DATA_LIST];
				bFlag = dataListJson.isArray();
				if(bFlag) {
					for(int i = 0; i < dataListJson.size(); i++) {
						VSSavedVideoListItem item;
						if (item.Parsing(dataListJson.get(i, Json::Value::null))) {
							list.push_back(item);
						}
					}
				}
			}

			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestVideoShowController::SavedVideoListCallbackHandle() parsing fail:"
						"(url:%s, size:%d, buf:%s)",
						url.c_str(), size, buf);
			}
		}
	}
	else {
		// request fail
		errnum = LOCAL_ERROR_CODE_TIMEOUT;
		errmsg = LOCAL_ERROR_CODE_TIMEOUT_DESC;
	}

	if( m_Callback.onRequestVSSavedVideoList != NULL ) {
		m_Callback.onRequestVSSavedVideoList(requestId, bFlag, errnum, errmsg, pageIndex, pageSize, dataCount, list);
	}
}
