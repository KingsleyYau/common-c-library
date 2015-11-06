/*
 * RequestAdvertController.h
 *
 *  Created on: 2015-04-22
 *      Author: Samson.Fan
 */

#ifndef REQUESTADVERTCONTROLLER_H_
#define REQUESTADVERTCONTROLLER_H_

#include "RequestBaseController.h"
#include "item/Advert.h"

typedef void (*OnRequestAdMainAdvert)(long requestId, bool success, const string& errnum, const string& errmsg, const AdMainAdvertItem& item);
typedef void (*OnRequestAdWomanListAdvert)(long requestId, bool success, const string& errnum, const string& errmsg, const AdWomanListAdvertItem& item);
typedef void (*OnRequestAdPushAdvert)(long requestId, bool success, const string& errnum, const string& errmsg, const AdPushAdvertList& pushList);
typedef struct _tagRequestAdvertControllerCallback {
	OnRequestAdMainAdvert onRequestAdMainAdvert;
	OnRequestAdWomanListAdvert onRequestAdWomanListAdvert;
	OnRequestAdPushAdvert onRequestAdPushAdvert;
} RequestAdvertControllerCallback;


class RequestAdvertController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestAdvertController(HttpRequestManager* pHttpRequestManager, const RequestAdvertControllerCallback& callback);
	virtual ~RequestAdvertController();

public:
	long MainAdvert(const string& deviceId, const string& advertId, int showTimes, int clickTimes);
	long WomanListAdvert(const string& deviceId, const string& advertId, int showTimes, int clickTimes);
	long PushAdvert(const string& deviceId, const string& pushId);

private:
	void MainAdvertCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void WomanListAdvertCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void PushAdvertCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);

// IHttpRequestManagerCallback interface
protected:
	virtual void onSuccess(long requestId, string path, const char* buf, int size);
	virtual void onFail(long requestId, string path);

private:
	RequestAdvertControllerCallback m_Callback;
};

#endif /* REQUESTADVERTCONTROLLER_H_ */
