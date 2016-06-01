/*
 * HttpRequestManager.cpp
 *
 *  Created on: 2015-2-28
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "HttpRequestManager.h"
#include <common/CheckMemoryLeak.h>

HttpRequestManager::HttpRequestManager() {
	// TODO Auto-generated constructor stub
	HttpClient::Init();
	mUser = "";
	mPassword = "";
}

HttpRequestManager::~HttpRequestManager() {
	// TODO Auto-generated destructor stub
}

//void HttpRequestManager::SetWebSite(string website) {
//	FileLog("httprequest", "HttpRequestManager::SetWebSite( website : %s )", website.c_str());
//	mWebSite = website;
//}
void HttpRequestManager::SetVersionCode(string versionKey, string versionCode) {
	mVersionKey = versionKey;
	mVersionCode = versionCode;
}

void HttpRequestManager::SetHostManager(HttpRequestHostManager *httpRequestHostManager) {
	mHttpRequestHostManager = httpRequestHostManager;
}

HttpRequestHostManager* HttpRequestManager::GetHostManager() {
	return mHttpRequestHostManager;
}

void HttpRequestManager::SetAuthorization(string user, string password) {
	FileLog("httprequest", "HttpRequestManager::SetAuthorization( user : %s, password : %s )",
			user.c_str(), password.c_str());
	mUser = user;
	mPassword = password;
}

unsigned long HttpRequestManager::StartRequest(string path, HttpEntiy& entiy, IHttpRequestManagerCallback* callback,
		SiteType type, bool bNoCache) {
	FileLog("httprequest", "HttpRequestManager::StartRequest( "
			"path : %s, "
			"entiy : %p, "
			"callback : %p )",
			path.c_str(),
			&entiy,
			callback
			);

	long requestId = 0;

	string host = mHttpRequestHostManager->GetHostByType(type);

	// 配置http用户认证
	entiy.SetAuthorization(mUser, mPassword);

	// 配置客户端版本
	entiy.AddHeader(mVersionKey, mVersionCode);

//	FileLog("httprequest", "HttpRequestManager::StartRequest() request lock begin, path:%s", path.c_str());
	mKMutex.lock();
//	FileLog("httprequest", "HttpRequestManager::StartRequest() request lock ok, path:%s", path.c_str());

	HttpRequest *request = new HttpRequest();
	request->SetCallback(this);
	request->SetNoCacheData(bNoCache);
	requestId = request->StartRequest(host, path, entiy);

	FileLog("httprequest", "HttpRequestManager::StartRequest( "
			"requestId : %ld, "
			"path : %s "
			") ",
			requestId,
			path.c_str()
			);

	mHttpRequestMap.insert(HttpRequestMap::value_type(requestId, request));
	if( callback != NULL ) {
		mIHttpRequestManagerCallbackMap.insert(IHttpRequestManagerCallbackMap::value_type(requestId, callback));
	}

//	FileLog("httprequest", "HttpRequestManager::StartRequest() request lock finish, path:%s", path.c_str());
	mKMutex.unlock();
//	FileLog("httprequest", "HttpRequestManager::StartRequest() request unlocked, path:%s", path.c_str());

	return requestId;
}

bool HttpRequestManager::StopRequest(unsigned long requestId, bool bWait) {
	FileLog("httprequest", "HttpRequestManager::StopRequest( requestId : %lu, bWait : %s )", requestId, bWait?"true":"false");

	bool bFlag = false;

	HttpRequest *request = NULL;
	mKMutex.lock();
	HttpRequestMap::iterator itr = mHttpRequestMap.find(requestId);
	if( itr != mHttpRequestMap.end() ) {

//		IHttpRequestManagerCallbackMap::iterator itr2 = mIHttpRequestManagerCallbackMap.find(requestId);
//		if( itr2 != mIHttpRequestManagerCallbackMap.end() ) {
//			mIHttpRequestManagerCallbackMap.erase(itr2);
//		}

		request = itr->second;
		request->StopRequest();
//		mHttpRequestMap.erase(itr);

		bFlag = true;
	}
	mKMutex.unlock();

//	if( request != NULL ) {
//		request->StopRequest(bWait);
//		delete request;
//	}

	return bFlag;
}

void HttpRequestManager::StopAllRequest() {
	FileLog("httprequest", "HttpRequestManager::StopAllRequest()");

	HttpRequest *request = NULL;
	mKMutex.lock();
	for(HttpRequestMap::iterator itr = mHttpRequestMap.begin(); itr != mHttpRequestMap.end(); itr++) {
		request = itr->second;

		if( request != NULL ) {
			request->StopRequest();
		}
	}
	mKMutex.unlock();
}

const HttpRequest* HttpRequestManager::GetRequestById(long requestId) {
	HttpRequest *request = NULL;
	HttpRequestMap::iterator itr = mHttpRequestMap.find(requestId);
	if( itr != mHttpRequestMap.end() ) {
		request = itr->second;
	}
	return request;
}

void HttpRequestManager::onSuccess(long requestId, string url, const char* buf, int size) {
	FileLog("httprequest", "HttpRequestManager::onSuccess( "
			"requestId : %ld, "
			"url : %s, "
			"buf( size : %d ) "
			")",
			requestId,
			url.c_str(),
			size
			);

	mKMutex.lock();
	string path;
	HttpRequest *request = NULL;

	HttpRequestMap::iterator itr2 = mHttpRequestMap.find(requestId);
	if( itr2 != mHttpRequestMap.end() ) {
		path = itr2->second->GetPath();
		IHttpRequestManagerCallbackMap::iterator itr = mIHttpRequestManagerCallbackMap.find(requestId);
		if( itr != mIHttpRequestManagerCallbackMap.end() ) {
			if( itr->second != NULL ) {
				itr->second->onSuccess(requestId, path, buf, size);
			}
			mIHttpRequestManagerCallbackMap.erase(itr);
		}

		request = itr2->second;
		if( request != NULL ) {
			delete request;
		}
		mHttpRequestMap.erase(itr2);
	}
	mKMutex.unlock();


//	FileLog("httprequest", "HttpRequestManager::onSuccess( "
//			"requestId : %ld, "
//			"url : %s, "
//			"buf( size : %d ) "
//			"exit )",
//			requestId,
//			url.c_str(),
//			size
//			);
}

void HttpRequestManager::onFail(long requestId, string url) {
	FileLog("httprequest", "HttpRequestManager::onFail( "
			"requestId : %ld, "
			"url : %s "
			")",
			requestId,
			url.c_str()
			);

	mKMutex.lock();
	string path;
	HttpRequest *request = NULL;

	HttpRequestMap::iterator itr2 = mHttpRequestMap.find(requestId);
	if( itr2 != mHttpRequestMap.end() ) {
		path = itr2->second->GetPath();

		IHttpRequestManagerCallbackMap::iterator itr = mIHttpRequestManagerCallbackMap.find(requestId);
		if( itr != mIHttpRequestManagerCallbackMap.end() ) {
			if( itr->second != NULL ) {
				itr->second->onFail(requestId, path);
			}
			mIHttpRequestManagerCallbackMap.erase(itr);
		}

		request = itr2->second;
		if( request != NULL ) {
			delete request;
		}
		mHttpRequestMap.erase(itr2);
	}

	mKMutex.unlock();

//	FileLog("httprequest", "HttpRequestManager::onFail( "
//			"requestId ; %ld, "
//			"url : %s, "
//			"exit "
//			")",
//			requestId,
//			url.c_str()
//			);
}

void HttpRequestManager::onReceiveBody(long requestId, string url, const char* buf, int size) {
//	FileLog("httprequest", "HttpRequestManager::onReceiveBody( "
//			"requestId : %ld, "
//			"url : %s"
//			" )",
//			requestId,
//			url.c_str()
//			);

	mKMutex.lock();
	string path;
	HttpRequestMap::iterator itr2 = mHttpRequestMap.find(requestId);
	if( itr2 != mHttpRequestMap.end() ) {
		path = itr2->second->GetPath();

		IHttpRequestManagerCallbackMap::iterator itr = mIHttpRequestManagerCallbackMap.find(requestId);
		if( itr != mIHttpRequestManagerCallbackMap.end() ) {
			itr->second->onReceiveBody(requestId, path, buf, size);
		}
	}

	mKMutex.unlock();

//	FileLog("httprequest", "HttpRequestManager::onReceiveBody( "
//			"requestId : %ld, "
//			"url : %s "
//			"exit "
//			")",
//			requestId,
//			url.c_str()
//			);
}
