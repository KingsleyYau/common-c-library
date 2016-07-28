/*
 * RequestAdvertController.cpp
 *
 *  Created on: 2015-3-6
 *      Author: Samson.Fan
 */

#include "RequestAdvertController.h"
#include "RequestDefine.h"
#include "RequestEMFDefine.h"
#include "../common/CommonFunc.h"

RequestAdvertController::RequestAdvertController(HttpRequestManager *pHttpRequestManager, const RequestAdvertControllerCallback& callback)
{
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(pHttpRequestManager);
	m_Callback = callback;
}

RequestAdvertController::~RequestAdvertController()
{
	// TODO Auto-generated destructor stub
}

/* IHttpRequestManagerCallback */
void RequestAdvertController::onSuccess(long requestId, string url, const char* buf, int size)
{
	FileLog("httprequest", "RequestAdvertController::onSuccess( url : %s, buf( size : %d ) )", url.c_str(), size);
	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestAdvertController::onSuccess(), buf: %s", buf);
	}

	if( url.compare(ADVERT_MAINADVERT_PATH) == 0 ) {
		MainAdvertCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(ADVERT_WOMANLISTADVERT_PATH) == 0 ) {
		WomanListAdvertCallbackHandle(requestId, url, true, buf, size);
	}
	else if( url.compare(ADVERT_PUSHADVERT_PATH) == 0 ) {
		PushAdvertCallbackHandle(requestId, url, true, buf, size);
	}else if( url.compare(ADVERT_APP_PROMOTION_PATH) == 0 ){
		AppPromotionCallbackHandle(requestId, url, true, buf, size);
	}
	FileLog("httprequest", "RequestAdvertController::onSuccess() end, url:%s", url.c_str());
}

void RequestAdvertController::onFail(long requestId, string url)
{
	FileLog("httprequest", "RequestAdvertController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(ADVERT_MAINADVERT_PATH) == 0 ) {
		MainAdvertCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(ADVERT_WOMANLISTADVERT_PATH) == 0 ) {
		WomanListAdvertCallbackHandle(requestId, url, false, NULL, 0);
	}
	else if( url.compare(ADVERT_PUSHADVERT_PATH) == 0 ) {
		PushAdvertCallbackHandle(requestId, url, false, NULL, 0);
	}else if( url.compare(ADVERT_APP_PROMOTION_PATH) == 0 ){
		AppPromotionCallbackHandle(requestId, url, false, NULL, 0);
	}
	FileLog("httprequest", "RequestAdvertController::onFail() end, url:%s", url.c_str());
}

// ----------------------- MainAdvert -----------------------
long RequestAdvertController::MainAdvert(const string& deviceId, const string& advertId, int showTimes, int clickTimes)
{
	char temp[16];
	HttpEntiy entiy;

	// deviceId
	entiy.AddContent(ADVERT_REQUEST_DEVICEID, deviceId);
	// advertId
	if (!advertId.empty()) {
		entiy.AddContent(ADVERT_REQUEST_ADVERTID, advertId);
		// showTimes
		sprintf(temp, "%d", showTimes);
		entiy.AddContent(ADVERT_REQUEST_SHOWTIMES, temp);
		// clickTimes
		sprintf(temp, "%d", clickTimes);
		entiy.AddContent(ADVERT_REQUEST_CLICKTIMES, temp);
	}

	string url = ADVERT_MAINADVERT_PATH;
	FileLog("httprequest", "RequestAdvertController::MainAdvert"
			"( url:%s, deviceId:%s, advertId:%s, showTimes:%d, clickTimes:%d)",
			url.c_str(), deviceId.c_str(), advertId.c_str(), showTimes, clickTimes);

	return StartRequest(url, entiy, this);
}

void RequestAdvertController::MainAdvertCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	AdMainAdvertItem item;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			if (dataJson.isObject()) {
				bFlag = item.Parsing(dataJson);
			}

			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestAdvertController::MainAdvertCallbackHandle() parsing fail:"
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

	if( m_Callback.onRequestAdMainAdvert != NULL ) {
		m_Callback.onRequestAdMainAdvert(requestId, bFlag, errnum, errmsg, item);
	}
}

// ----------------------- WomanListAdvert -----------------------
long RequestAdvertController::WomanListAdvert(const string& deviceId, const string& advertId, int showTimes, int clickTimes)
{
	char temp[16];
	HttpEntiy entiy;

	// deviceId
	entiy.AddContent(ADVERT_REQUEST_DEVICEID, deviceId);
	// advertId
	if (!advertId.empty()) {
		entiy.AddContent(ADVERT_REQUEST_ADVERTID, advertId);
		// showTimes
		sprintf(temp, "%d", showTimes);
		entiy.AddContent(ADVERT_REQUEST_SHOWTIMES, temp);
		// clickTimes
		sprintf(temp, "%d", clickTimes);
		entiy.AddContent(ADVERT_REQUEST_CLICKTIMES, temp);
	}

	string url = ADVERT_WOMANLISTADVERT_PATH;
	FileLog("httprequest", "RequestAdvertController::WomanListAdvert"
			"( url:%s, deviceId:%s, advertId:%s, showTimes:%d, clickTimes:%d)",
			url.c_str(), deviceId.c_str(), advertId.c_str(), showTimes, clickTimes);

	return StartRequest(url, entiy, this);
}

void RequestAdvertController::WomanListAdvertCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	AdWomanListAdvertItem item;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			if (dataJson.isObject()) {
				bFlag = item.Parsing(dataJson);
			}

			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestAdvertController::WomanListAdvertCallbackHandle() parsing fail:"
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

	if( m_Callback.onRequestAdWomanListAdvert != NULL ) {
		m_Callback.onRequestAdWomanListAdvert(requestId, bFlag, errnum, errmsg, item);
	}
}

// ----------------------- PushAdvert -----------------------
long RequestAdvertController::PushAdvert(const string& deviceId, const string& pushId)
{
//	char temp[16];
	HttpEntiy entiy;

	// deviceId
	entiy.AddContent(ADVERT_REQUEST_DEVICEID, deviceId);
	// pushId
	if (!pushId.empty()) {
		entiy.AddContent(ADVERT_REQUEST_PUSHID, pushId);
	}

	string url = ADVERT_PUSHADVERT_PATH;
	FileLog("httprequest", "RequestAdvertController::PushAdvert"
			"( url:%s, deviceId:%s, pushId:%s)",
			url.c_str(), deviceId.c_str(), pushId.c_str());

	return StartRequest(url, entiy, this);
}

void RequestAdvertController::PushAdvertCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	AdPushAdvertList list;
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			if (dataJson[COMMON_DATA_LIST].isArray()) {
				bFlag = true;

				int i = 0;
				for (i = 0; i < dataJson[COMMON_DATA_LIST].size(); i++) {
					AdPushAdvertItem item;
					if (item.Parsing(dataJson[COMMON_DATA_LIST].get(i, Json::Value::null))) {
						list.push_back(item);
					}
				}
			}

			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestAdvertController::WomanListAdvertCallbackHandle() parsing fail:"
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

	if( m_Callback.onRequestAdPushAdvert != NULL ) {
		m_Callback.onRequestAdPushAdvert(requestId, bFlag, errnum, errmsg, list);
	}
}

// ----------------------- AppPromotionAdvert -----------------------
long RequestAdvertController::AppPromotionAdvert(const string& deviceId)
{
//	char temp[16];
	HttpEntiy entiy;

	// deviceId
	entiy.AddContent(ADVERT_REQUEST_DEVICEID, deviceId);

	string url = ADVERT_APP_PROMOTION_PATH;
	FileLog("httprequest", "RequestAdvertController::AppPromotionAdvert"
			"( url:%s, deviceId:%s)",url.c_str(), deviceId.c_str());

	return StartRequest(url, entiy, this);
}

void RequestAdvertController::AppPromotionCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size)
{
	string adOverview = "";
	string errnum = "";
	string errmsg = "";
	bool bFlag = false;

	if (requestRet) {
		// request success
		Json::Value dataJson;
		if( HandleResult(buf, size, errnum, errmsg, &dataJson) ) {
			if (dataJson[ADVERT_ADOVERVIEW].isString()) {
				adOverview = dataJson[ADVERT_ADOVERVIEW].asString();
				bFlag = true;
			}
			if (!bFlag) {
				// parsing fail
				errnum = LOCAL_ERROR_CODE_PARSEFAIL;
				errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;

				FileLog("httprequest", "RequestAdvertController::AppPromotionCallbackHandle() parsing fail:"
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

	if( m_Callback.onRequestAppPromotionAdvert != NULL ) {
		m_Callback.onRequestAppPromotionAdvert(requestId, bFlag, errnum, errmsg, adOverview);
	}
}
