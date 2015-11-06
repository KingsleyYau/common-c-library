/*
 * RequestBaseController.h
 *
 *  Created on: 2015-3-2
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef RequestBaseController_H_
#define RequestBaseController_H_

#include <map>
using namespace std;

#include "HttpRequestManager.h"

#include <json/json/json.h>
#include <common/KSafeMap.h>

#include <xml/tinyxml.h>

#include "RequestDefine.h"

// add by samson 2015-04-20
typedef map<long, void*> RequestCustomParamMap;

typedef KSafeMap<string, long> RequestFileDownloadMap;

class RequestBaseController {
public:
	RequestBaseController();
	virtual ~RequestBaseController();

	void SetHttpRequestManager(HttpRequestManager* pHttpRequestManager);

	long StartRequest(string url, HttpEntiy& entiy, IHttpRequestManagerCallback* callback = NULL, SiteType type = AppSite, bool bNoCache = false);
	bool StopRequest(long requestId, bool bWait = false);

protected:
	bool HandleResult(const char* buf, int size, string &errnum, string &errmsg, Json::Value *data, Json::Value *errdata = NULL);
	bool HandleResult(const char* buf, int size, string &errnum, string &errmsg, TiXmlDocument &doc);

	/**
	 * 必须在回调完调用, 回调完成后获取不了
	 */
	string GetContentTypeById(long requestId);

	// 获取body总数及已下载数
	void GetRecvLength(long requestId, int& total, int& recv);

private:
	HttpRequestManager* mpHttpRequestManager;

protected:
	RequestCustomParamMap	mCustomParamMap;
	RequestFileDownloadMap  mRequestFileDownloadMap;
};

#endif /* RequestBaseController_H_ */
