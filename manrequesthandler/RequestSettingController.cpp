/*
 * RequestSettingController.cpp
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "RequestSettingController.h"

RequestSettingController::RequestSettingController(HttpRequestManager *pHttpRequestManager, RequestSettingControllerCallback callback/* CallbackManager* pCallbackManager*/) {
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(pHttpRequestManager);
	mRequestSettingControllerCallback = callback;
}

RequestSettingController::~RequestSettingController() {
	// TODO Auto-generated destructor stub
}

/* IHttpRequestManagerCallback */
void RequestSettingController::onSuccess(long requestId, string url, const char* buf, int size) {
	FileLog("httprequest", "RequestSettingController::onSuccess( url : %s, buf( size : %d ) )", url.c_str(), size);
	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestSettingController::onSuccess(), buf: %s", buf);
	}

	/* parse base result */
	string errnum = "";
	string errmsg = "";
	Json::Value data;

	bool bFlag = HandleResult(buf, size, errnum, errmsg, &data);

	/* resopned parse ok, callback success */
	if( url.compare(CHANGE_PASSWORD_PATH) == 0 ) {
		/*  2.1.查询个人信息 */
		if( mRequestSettingControllerCallback.onChangePassword != NULL ) {
			mRequestSettingControllerCallback.onChangePassword(requestId, bFlag, errnum, errmsg);
		}
	}
	FileLog("httprequest", "RequestSettingController::onSuccess() end, url:%s", url.c_str());
}
void RequestSettingController::onFail(long requestId, string url) {
	FileLog("httprequest", "RequestSettingController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(CHANGE_PASSWORD_PATH) == 0 ) {
		/* 2.1.查询个人信息 */
		if( mRequestSettingControllerCallback.onChangePassword != NULL ) {
			mRequestSettingControllerCallback.onChangePassword(requestId, false,
					LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	}
	FileLog("httprequest", "RequestSettingController::onFail() end, url:%s", url.c_str());
}

/**
 * 3.1.修改密码
 * @param oldPassword	新密码
 * @param newPassword	旧密码
 * @param callback
 * @return				请求唯一标识
 */
long RequestSettingController::ChangePassword(string oldPassword, string newPassword) {
	HttpEntiy entiy;

	if( oldPassword.length() > 0 ) {
		entiy.AddContent(SETTING_PASSWORDOLD, oldPassword);
	}

	if( newPassword.length() > 0 ) {
		entiy.AddContent(SETTING_PASSWORDNEW, newPassword);
	}

	string url = CHANGE_PASSWORD_PATH;
	FileLog("httprequest", "RequestSettingController::ChangePassword( "
			"url : %s, "
			"oldPassword : %s "
			"newPassword : %s ",
			url.c_str(),
			oldPassword.c_str(),
			newPassword.c_str()
			);

	return StartRequest(url, entiy, this);
}
