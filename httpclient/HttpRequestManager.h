/*
 * HttpRequestManager.h
 *
 *  Created on: 2015-2-28
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef HTTPREQUESTMANAGER_H_
#define HTTPREQUESTMANAGER_H_

#include <map>
using namespace std;

#include <common/KMutex.h>

#include "HttpRequest.h"

#include "HttpRequestHostManager.h"

class IHttpRequestManagerCallback {
public:
	virtual ~IHttpRequestManagerCallback(){};
	virtual void onSuccess(long requestId, string path, const char* buf, int size) = 0;
	virtual void onFail(long requestId, string path) = 0;
	virtual void onReceiveBody(long requestId, string path, const char* buf, int size){};
};

typedef map<unsigned long, HttpRequest*> HttpRequestMap;
typedef map<unsigned long, IHttpRequestManagerCallback*> IHttpRequestManagerCallbackMap;

class HttpRequestManager : public IHttpRequestCallback {
public:
	HttpRequestManager();
	virtual ~HttpRequestManager();

//	void SetWebSite(string website);

	/**
	 * 设置客户端版本号
	 */
	void SetVersionCode(string versionKey, string versionCode);

	/**
	 * 设置站点管理实例
	 */
	void SetHostManager(HttpRequestHostManager *httpRequestHostManager);

	/**
	 * 获取站点管理实例
	 */
	HttpRequestHostManager* GetHostManager();

	/**
	 * 设置认证信息
	 */
	void SetAuthorization(string user, string password);

	/**
	 * 开始发起请求
	 * @param path		请求路径
	 * @param entiy		请求体
	 * @param callback	回调实例
	 * @param type		站点类型
	 * @param bNoCache	是否不缓存结果, 如果是, 可通过onReceiveBody处理自定义数据, onSuccess没有数据返回
	 * 							           如果否, 在onSuccess可以获得缓存的所有结果
	 * @return 请求唯一Id,	用于停止
	 */
	unsigned long StartRequest(string path, HttpEntiy& entiy, IHttpRequestManagerCallback* callback = NULL,
			SiteType type = AppSite, bool bNoCache = false);

	/**
	 * 停止请求
	 * @param requestId	请求Id
	 */
	bool StopRequest(unsigned long requestId, bool bWait = false);

	/**
	 * 停止请求
	 */
	void StopAllRequest();

	/**
	 * 获取请求实例
	 * 没有加锁, 只在回调里面调用
	 * @param requestId	请求Id
	 * @return 请求实例
	 */
	const HttpRequest* GetRequestById(long requestId);

protected:
	void onSuccess(long requestId, string url, const char* buf, int size);
	void onFail(long requestId, string url);
	void onReceiveBody(long requestId, string url, const char* buf, int size);
private:
//	string mWebSite;
	HttpRequestHostManager *mHttpRequestHostManager;
	string mUser;
	string mPassword;
	string mVersionKey;
	string mVersionCode;

	HttpRequestMap mHttpRequestMap;
	IHttpRequestManagerCallbackMap mIHttpRequestManagerCallbackMap;
	KMutex mKMutex;
};

#endif /* HTTPREQUESTMANAGER_H_ */
