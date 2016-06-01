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

class IRequestOtherControllerCallback
{
public:
	IRequestOtherControllerCallback() {}
	virtual ~IRequestOtherControllerCallback() {}
public:
	virtual void OnEmotionConfig(long requestId, bool success, const string& errnum, const string& errmsg, const OtherEmotionConfigItem& item) {};
	virtual void OnGetCount(long requestId, bool success, const string& errnum, const string& errmsg, const OtherGetCountItem& item) {};
	virtual void OnPhoneInfo(long requestId, bool success, const string& errnum, const string& errmsg) {};
	virtual void OnIntegralCheck(long requestId, bool success, const string& errnum, const string& errmsg, const OtherIntegralCheckItem& item) {};
	virtual void OnVersionCheck(long requestId, bool success, const string& errnum, const string& errmsg, const OtherVersionCheckItem& item) {};
	virtual void OnSynConfig(long requestId, bool success, const string& errnum, const string& errmsg, const OtherSynConfigItem& item) {};
	virtual void OnOnlineCount(long requestId, bool success, const string& errnum, const string& errmsg, const OtherOnlineCountList& countList) {};
	virtual void OnUploadCrashLog(long requestId, bool success, const string& errnum, const string& errmsg) {};
	virtual void OnInstallLogs(long requestId, bool success, const string& errnum, const string& errmsg) {};
};


class RequestOtherController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestOtherController(HttpRequestManager* pHttpRequestManager, IRequestOtherControllerCallback* callback);
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
#ifdef _ANDROID
	long InstallLogs(const string& deviceId, long installtime, long submittime, int verCode
			, const string& model, const string& manufacturer, const string& os, const string& release
			, const string& sdk, int width, int height, const string& referrer, bool isSimulator, const string& checkInfo);
#endif

private:
	void EmotionConfigCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void GetCountCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void PhoneInfoCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void IntegralCheckCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void VersionCheckCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void SynConfigCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void OnlineCountCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
	void UploadCrashLogCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
#ifdef _ANDROID
	void InstallLogsCallbackHandle(long requestId, const string& url, bool requestRet, const char* buf, int size);
#endif
protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	IRequestOtherControllerCallback* m_Callback;
};

#endif /* REQUESTOTHERCONTROLLER_H_ */
