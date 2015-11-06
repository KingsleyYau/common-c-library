/*
 * RequestQuickMatchController.cpp
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "RequestQuickMatchController.h"

RequestQuickMatchController::RequestQuickMatchController(HttpRequestManager *phttprequestManager, RequestQuickMatchControllerCallback callback/* CallbackManager* pCallbackManager*/) {
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(phttprequestManager);
	mRequestQuickMatchControllerCallback = callback;
}

RequestQuickMatchController::~RequestQuickMatchController() {
	// TODO Auto-generated destructor stub
}

/* IhttprequestManagerCallback */
void RequestQuickMatchController::onSuccess(long requestId, string url, const char* buf, int size) {
	FileLog("httprequest", "RequestQuickMatchController::onSuccess( url : %s, buf( size : %d ) )", url.c_str(), size);
	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestQuickMatchController::onSuccess(), buf: %s", buf);
	}

	/* parse base result */
	string errnum = "";
	string errmsg = "";
	Json::Value data;

	bool bFlag = HandleResult(buf, size, errnum, errmsg, &data);

	/* resopned parse ok, callback success */
	if( url.compare(QUICKMATCH_LIST_PATH) == 0 ) {
		/* 10.1.查询女士图片列表  */
		list<QuickMatchLady> itemList;
		if( data[COMMON_DATA_LIST].isArray() ) {
			for(int i = 0; i < data[COMMON_DATA_LIST].size(); i++ ) {
				QuickMatchLady item;
				item.Parse(data[COMMON_DATA_LIST].get(i, Json::Value::null));
				itemList.push_back(item);
			}
		} else {
			// parsing fail
			bFlag = false;
			errnum = LOCAL_ERROR_CODE_PARSEFAIL;
			errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;
		}

		if( mRequestQuickMatchControllerCallback.onQueryQuickMatchLadyList != NULL ) {
			mRequestQuickMatchControllerCallback.onQueryQuickMatchLadyList(requestId, bFlag,
					itemList, errnum, errmsg);
		}
	} else if( url.compare(QUICKMATCH_UPLOAD_PATH) == 0 ) {
		/* 10.2.提交已标记的女士 */
		if( mRequestQuickMatchControllerCallback.onSubmitQuickMatchMarkLadyList != NULL ) {
			mRequestQuickMatchControllerCallback.onSubmitQuickMatchMarkLadyList(requestId, bFlag, errnum, errmsg);
		}
	} else if( url.compare(QUICKMATCH_LIKE_LIST_PATH) == 0 ) {
		/* 10.3.查询已标记like的女士列表 */
		list<QuickMatchLady> itemList;
		int totalCount = 0;
		if( data[COMMON_DATA_COUNT].isInt() ) {
			totalCount = data[COMMON_DATA_COUNT].asInt();
		}
		if( data[COMMON_DATA_LIST].isArray() ) {
			for(int i = 0; i < data[COMMON_DATA_LIST].size(); i++ ) {
				QuickMatchLady item;
				item.Parse(data[COMMON_DATA_LIST].get(i, Json::Value::null));
				itemList.push_back(item);
			}
		} else {
			// parsing fail
			bFlag = false;
			errnum = LOCAL_ERROR_CODE_PARSEFAIL;
			errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;
		}

		if( mRequestQuickMatchControllerCallback.onQueryQuickMatchLikeLadyList != NULL ) {
			mRequestQuickMatchControllerCallback.onQueryQuickMatchLikeLadyList(requestId, bFlag, itemList,
					totalCount, errnum, errmsg);
		}
	} else if( url.compare(QUICKMATCH_REMOVE_PATH) == 0 ) {
		/* 10.4.删除已标记like的女士 */
		if( mRequestQuickMatchControllerCallback.onSubmitQuickMatchMarkLadyList != NULL ) {
			mRequestQuickMatchControllerCallback.onSubmitQuickMatchMarkLadyList(requestId, bFlag, errnum, errmsg);
		}
	}
	FileLog("httprequest", "RequestQuickMatchController::onSuccess() end, url:%s", url.c_str());
}
void RequestQuickMatchController::onFail(long requestId, string url) {
	FileLog("httprequest", "RequestQuickMatchController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(QUICKMATCH_LIST_PATH) == 0 ) {
		/* 10.1.查询女士图片列表   */
		list<QuickMatchLady> itemList;
		if( mRequestQuickMatchControllerCallback.onQueryQuickMatchLadyList != NULL ) {
			mRequestQuickMatchControllerCallback.onQueryQuickMatchLadyList(requestId, false, itemList, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(QUICKMATCH_UPLOAD_PATH) == 0 ) {
		/* 10.2.提交已标记的女士 */
		if( mRequestQuickMatchControllerCallback.onSubmitQuickMatchMarkLadyList != NULL ) {
			mRequestQuickMatchControllerCallback.onSubmitQuickMatchMarkLadyList(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(QUICKMATCH_LIKE_LIST_PATH) == 0 ) {
		/* 10.3.查询已标记like的女士列表 */
		list<QuickMatchLady> itemList;
		if( mRequestQuickMatchControllerCallback.onQueryQuickMatchLikeLadyList != NULL ) {
			mRequestQuickMatchControllerCallback.onQueryQuickMatchLikeLadyList(requestId, false, itemList,
					0, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(QUICKMATCH_REMOVE_PATH) == 0 ) {
		/* 10.4.删除已标记like的女士 */
		if( mRequestQuickMatchControllerCallback.onSubmitQuickMatchMarkLadyList != NULL ) {
			mRequestQuickMatchControllerCallback.onSubmitQuickMatchMarkLadyList(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	}
	FileLog("httprequest", "RequestQuickMatchController::onFail() end, url:%s", url.c_str());
}

/**
 * 10.1.查询女士图片列表
 * @param deviceId		设备唯一标识
 * @return				请求唯一标识
 */
long RequestQuickMatchController::QueryQuickMatchLadyList(string deviceId) {
	HttpEntiy entiy;

	entiy.AddContent(QUICKMATCH_QUERY_DEVICEID, deviceId.c_str());

	string url = QUICKMATCH_LIST_PATH;
	FileLog("httprequest", "RequestQuickMatchController::QueryQuickMatchLadyList( "
			"url : %s, "
			"deviceId : %s "
			")",
			url.c_str(),
			deviceId.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 10.2.提交已标记的女士
 * @param likeList		喜爱的女士列表
 * @param unlikeList	不喜爱女士列表
 * @return				请求唯一标识
 */
long RequestQuickMatchController::SubmitQuickMatchMarkLadyList(list<string> likeListId, list<string> unlikeListId) {
	HttpEntiy entiy;

	string likeId = "";
	for(list<string>::iterator itr = likeListId.begin(); itr != likeListId.end(); itr++) {
		likeId += *itr;
		likeId += ",";
	}
	if( likeId.length() > 0 ) {
		likeId = likeId.substr(0, likeId.length() - 1);
		entiy.AddContent(QUICKMATCH_UPLOAD_CONFIRMTYPE, likeId);
	}

	string unlikeId = "";
	for(list<string>::iterator itr = unlikeListId.begin(); itr != unlikeListId.end(); itr++) {
		unlikeId += *itr;
		unlikeId += ",";
	}
	if( unlikeId.length() > 0 ) {
		unlikeId = unlikeId.substr(0, unlikeId.length() - 1);
		entiy.AddContent(QUICKMATCH_UPLOAD_CONFIRMTYPE, unlikeId);
	}

	string url = QUICKMATCH_UPLOAD_PATH;
	FileLog("httprequest", "RequestQuickMatchController::SubmitQuickMatchMarkLadyList( "
			"url : %s, "
			"likeId : %s, "
			"unlikeId : %s "
			")",
			url.c_str(),
			likeId.c_str(),
			unlikeId.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 10.3.查询已标记like的女士列表
 * @return				请求唯一标识
 */
long RequestQuickMatchController::QueryQuickMatchLikeLadyList(int pageIndex, int pageSize) {
	char temp[16];

	HttpEntiy entiy;

	sprintf(temp, "%d", pageIndex);
	entiy.AddContent(COMMON_PAGE_INDEX, temp);

	sprintf(temp, "%d", pageSize);
	entiy.AddContent(COMMON_PAGE_SIZE, temp);

	string url = QUICKMATCH_LIKE_LIST_PATH;
	FileLog("httprequest", "RequestQuickMatchController::QueryQuickMatchLikeLadyList( "
			"url : %s, "
			"pageIndex : %d, "
			"pageSize : %d "
			")",
			url.c_str(),
			pageIndex,
			pageSize
			);

	return StartRequest(url, entiy, this);
}

/**
 * 10.4.删除已标记like的女士
 * @param likeListId	喜爱的女士列表
 * @return				请求唯一标识
 */
long RequestQuickMatchController::RemoveQuickMatchLikeLadyList(list<string> likeListId) {

	HttpEntiy entiy;

	string likeId = "";
	for(list<string>::iterator itr = likeListId.begin(); itr != likeListId.end(); itr++) {
		likeId += *itr;
		likeId += ",";
	}
	if( likeId.length() > 0 ) {
		likeId = likeId.substr(0, likeId.length() - 1);
		entiy.AddContent(QUICKMATCH_UPLOAD_CONFIRMTYPE, likeId);
	}

	string url = QUICKMATCH_REMOVE_PATH;
	FileLog("httprequest", "RequestQuickMatchController::RemoveQuickMatchLikeLadyList( "
			"url : %s, "
			"likeId : %s "
			")",
			url.c_str(),
			likeId.c_str()
			);

	return StartRequest(url, entiy, this);
}
