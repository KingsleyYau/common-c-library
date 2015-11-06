/*
 * RequestOtherController.h
 *
 *  Created on: 2015-3-6
 *      Author: Samson.Fan
 */

#ifndef REQUESTOTHERCONTROLLER_H_
#define REQUESTOTHERCONTROLLER_H_

#include "RequestBaseController.h"
#include "item/Other.h"

typedef void (*OnRequestOtherEmotionConfig)(long requestId, bool success, const string& errnum, const string& errmsg, const OtherEmotionConfigItem& item);
typedef void (*OnRequestOtherGetCount)(long requestId, bool success, const string& errnum, const string& errmsg, const OtherGetCountItem& item);
typedef void (*OnRequestOtherPhoneInfo)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef void (*OnRequestOtherIntegralCheck)(long requestId, bool success, const string& errnum, const string& errmsg, const OtherIntegralCheckItem& item);
typedef void (*OnRequestOtherVersionCheck)(long requestId, bool success, const string& errnum, const string& errmsg, const OtherVersionCheckItem& item);
typedef void (*OnRequestOtherSynConfig)(long requestId, bool success, const string& errnum, const string& errmsg, const OtherSynConfigItem& item);
typedef void (*OnRequestOtherOnlineCount)(long requestId, bool success, const string& errnum, const string& errmsg, const OtherOnlineCountList& countList);
typedef void (*OnRequestOtherUploadCrashLog)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef void (*OnRequestOtherInstallLogs)(long requestId, bool success, const string& errnum, const string& errmsg);
typedef struct _tagRequestOtherControllerCallback {
	OnRequestOtherEmotionConfig onRequestOtherEmotionConfig;
	OnRequestOtherGetCount onRequestOtherGetCount;
	OnRequestOtherPhoneInfo onRequestOtherPhoneInfo;
	OnRequestOtherIntegralCheck onRequestOtherIntegralCheck;
	OnRequestOtherVersionCheck onRequestOtherVersionCheck;
	OnRequestOtherSynConfig onRequestOtherSynConfig;
	OnRequestOtherOnlineCount onRequestOtherOnlineCount;
	OnRequestOtherUploadCrashLog onRequestOtherUploadCrashLog;
	OnRequestOtherInstallLogs onRequestOtherInstallLogs;
} RequestOtherControllerCallback;


class RequestOtherController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestOtherController(HttpRequestManager* pHttpRequestManager, const RequestOtherControllerCallback& callback);
	virtual ~RequestOtherController();

public:
	long EmotionConfig();
	long GetCount(bool money, bool coupon, bool regStep, bool allowAlbum, bool admirerUr, bool integral);
	long PhoneInfo(const string& manId, int verCode, const string& verName, int action, int siteId
			, double density, int width, int height, const string& densityDpi, const string& model, const string& manufacturer, const string& os
			, const string& release, const string& sdk, const string& language, const string& region
			, const string& lineNumber, const string& simOptName, const string& simOpt, const string& simCountryIso, const string& simState
			, int phoneType, int networkType, const string& deviceId);
	long IntegralCheck(const string& womanId);
	long VersionCheck(int currVer);
	long SynConfig();
	long OnlineCount(int site);
	long UploadCrashLog(const string& deviceId, const string& file);
	long InstallLogs(const string& deviceId, long installtime, long submittime, int verCode
			, const string& model, const string& manufacturer, const string& os, const string& release
			, const string& sdk, int width, int height, const string& referrer, bool isSimulator, const string& checkInfo);

private:
	void EmotionConfigCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void GetCountCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void PhoneInfoCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void IntegralCheckCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void VersionCheckCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void SynConfigCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void OnlineCountCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void UploadCrashLogCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void InstallLogsCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	RequestOtherControllerCallback m_Callback;
};

#endif /* REQUESTOTHERCONTROLLER_H_ */
