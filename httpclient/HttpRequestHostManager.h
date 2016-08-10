/*
 * HttpRequestHostManager.h
 *
 *  Created on: 2015-3-12
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef HTTPREQUESTHOSTMANAGER_H_
#define HTTPREQUESTHOSTMANAGER_H_

#include <string>
using namespace std;

#include "../common/KLog.h"

typedef enum SiteType {
	AppSite,		// app站点
	WebSite,		// www站点
	ChatVoiceSite,	// 语音文件站点
	TransSite,		// 翻译站点
	VideoUploadSite,// 视频上传站点
} SiteType;

class HttpRequestHostManager {
public:
	HttpRequestHostManager();
	virtual ~HttpRequestHostManager();

	string GetHostByType(SiteType type);
	void SetWebSite(const string& webSite);
	string GetWebSite();
	void SetAppSite(const string& appSite);
	string GetAppSite();
	void SetChatVoiceSite(const string& chatVoiceSite);
	string GetChatVoiceSite();
	void SetTransSite(const string& transSite);
	string GetTransSite();
	void SetVideoUploadSite(const string& videoUploadSite);
	string GetVideoUploadSite();

private:
	string mWebSite;
	string mAppSite;
	string mChatVoiceSite;
	string mTransSite;
	string mVideoUploadSite;
};

#endif /* HTTPREQUESTHOSTMANAGER_H_ */
