/*
 * RequestMonthlyFeeController.cpp
 *
 *  Created on: 2016-5-10
 *      Author: Hunter
 */

#include "RequestMonthlyFeeController.h"

RequestMonthlyFeeController::RequestMonthlyFeeController(HttpRequestManager *phttprequestManager, RequestMonthlyFeeControllerCallback callback/* CallbackManager* pCallbackManager*/) {
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(phttprequestManager);
	mRequestMonthlyFeeControllerCallback = callback;
}

RequestMonthlyFeeController::~RequestMonthlyFeeController() {
	// TODO Auto-generated destructor stub
}

/* IhttprequestManagerCallback */
void RequestMonthlyFeeController::onSuccess(long requestId, string url, const char* buf, int size) {
	FileLog("httprequest", "RequestMonthlyFeeController::onSuccess( url : %s, buf( size : %d ) )", url.c_str(), size);
	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestMonthlyFeeController::onSuccess(), buf: %s", buf);
	}

	/* parse base result */
	string errnum = "";
	string errmsg = "";
	Json::Value data;

	bool bFlag = HandleResult(buf, size, errnum, errmsg, &data);

	/* resopned parse ok, callback success */
	if( url.compare(MONTHLY_FEE_MEMBER_TYPE_PATH) == 0 ) {
		/* 13.1.获取月费会员类型   */
		int memberType = 0;
		if( data[MONTHLY_FEE_MEMBER_TYPE].isInt() ){
			memberType = data[MONTHLY_FEE_MEMBER_TYPE].asInt();
		}
		if( mRequestMonthlyFeeControllerCallback.onQueryMemberType != NULL ) {
			mRequestMonthlyFeeControllerCallback.onQueryMemberType(requestId, bFlag, errnum, errmsg, memberType);
		}
	} else if( url.compare(MONTHLY_FEE_GET_TIPS_PATH) == 0 ) {
		/* 13.2.获取月费提示层数据   */
		list<MonthlyFeeTip> itemList;
		if( data.isArray() ) {
			for(int i = 0; i < data.size(); i++ ) {
				MonthlyFeeTip item;
				item.Parse(data.get(i, Json::Value::null));
				itemList.push_back(item);
			}
		} else {
			// parsing fail
			bFlag = false;
			errnum = LOCAL_ERROR_CODE_PARSEFAIL;
			errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;
		}
		if( mRequestMonthlyFeeControllerCallback.onGetMonthlyFeeTips != NULL ) {
			mRequestMonthlyFeeControllerCallback.onGetMonthlyFeeTips(requestId, bFlag, errnum, errmsg, itemList);
		}
	}
	FileLog("httprequest", "RequestMonthlyFeeController::onSuccess() end, url:%s", url.c_str());
}
void RequestMonthlyFeeController::onFail(long requestId, string url) {
	FileLog("httprequest", "RequestMonthlyFeeController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(MONTHLY_FEE_MEMBER_TYPE_PATH) == 0 ) {
		/* 13.1.获取月费会员类型  */
		if( mRequestMonthlyFeeControllerCallback.onQueryMemberType != NULL ) {
			mRequestMonthlyFeeControllerCallback.onQueryMemberType(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC, 0);
		}
	} else if( url.compare(MONTHLY_FEE_GET_TIPS_PATH) == 0 ) {
		/* 13.2.获取月费提示层数据   */
		list<MonthlyFeeTip> itemList;
		if( mRequestMonthlyFeeControllerCallback.onGetMonthlyFeeTips != NULL ) {
			mRequestMonthlyFeeControllerCallback.onGetMonthlyFeeTips(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC, itemList);
		}
	}
	FileLog("httprequest", "RequestMonthlyFeeController::onFail() end, url:%s", url.c_str());
}

/**
 * 13.1.获取月费会员类型
 * @return					请求唯一标识
 */
long RequestMonthlyFeeController::QueryMemberType() {
	HttpEntiy entiy;

	string url = MONTHLY_FEE_MEMBER_TYPE_PATH;
	FileLog("httprequest", "RequestMonthlyFeeController::QueryMemberType( "
			"url : %s"
			")",
			url.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 13.2.获取月费提示层数据
 * @return					请求唯一标识
 */
long RequestMonthlyFeeController::GetMonthlyFeeTips() {
	HttpEntiy entiy;

	string url = MONTHLY_FEE_GET_TIPS_PATH;
	FileLog("httprequest", "RequestMonthlyFeeController::GetMonthlyFeeTips( "
			"url : %s"
			")",
			url.c_str()
			);

	return StartRequest(url, entiy, this);
}
