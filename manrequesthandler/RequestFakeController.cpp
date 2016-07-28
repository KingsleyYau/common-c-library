/*
 * RequestFakeController.cpp
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "RequestFakeController.h"

#include "RequestEnumDefine.h"

#include <amf/AmfParser.h>

RequestFakeController::RequestFakeController(HttpRequestManager *pHttpRequestManager, IRequestFakeControllerCallback* pRequestFakeControllerCallback/* CallbackManager* pCallbackManager*/) {
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(pHttpRequestManager);
	mpRequestFakeControllerCallback = pRequestFakeControllerCallback;
}

RequestFakeController::~RequestFakeController() {
	// TODO Auto-generated destructor stub
}

/* IHttpRequestManagerCallback */
void RequestFakeController::onSuccess(long requestId, string url, const char* buf, int size) {
	FileLog("httprequest",
			"RequestFakeController::onSuccess( url : %s, content-type : %s, buf( size : %d ) )",
			url.c_str(),
			GetContentTypeById(requestId).c_str(),
			size
			);

	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestFakeController::onSuccess(), buf: %s", buf);
	}

	/* parse base result */
	string errnum = "";
	string errmsg = "";
	Json::Value data;
	Json::Value errdata;

	bool bFlag = HandleResult(buf, size, errnum, errmsg, &data, &errdata);

	/* resopned parse ok, callback success */
	if( url.compare(FAKE_CHECK_SERVER_PATH) == 0 ) {
		/* 2.1.检查真假服务器 */
		CheckServerItem item;
		item.Parse(data);

		if( mpRequestFakeControllerCallback != NULL ) {
			mpRequestFakeControllerCallback->OnCheckServer(requestId, bFlag, item, errnum, errmsg);
		}
	}

	FileLog("httprequest", "RequestFakeController::onSuccess() end, url:%s", url.c_str());
}
void RequestFakeController::onFail(long requestId, string url) {
	FileLog("httprequest", "RequestFakeController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(FAKE_CHECK_SERVER_PATH) == 0 ) {
		/* 2.1.检查真假服务器 */
		CheckServerItem item;

		if( mpRequestFakeControllerCallback != NULL ) {
			mpRequestFakeControllerCallback->OnCheckServer(requestId, false, item,
					LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	}
	FileLog("httprequest", "RequestFakeController::onFail() end, url:%s", url.c_str());
}


/**
 * 2.1.检查真假服务器
 * @param email				电子邮箱
 * @return					请求唯一标识
 */
long RequestFakeController::CheckServer(
		string email
		) {

	HttpEntiy entiy;

	if( email.length() > 0 ) {
		entiy.AddContent(FAKE_EMAIL, email.c_str());
	}

	string url = FAKE_CHECK_SERVER_PATH;
	FileLog("httprequest", "RequestFakeController::LoginWithFacebook( "
			"url : %s, "
			"email : %s "
			" )",
			url.c_str(),
			email.c_str()
			);

	return StartRequest(url, entiy, this, FakeSite);
}
