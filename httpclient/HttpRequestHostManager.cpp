/*
 * HttpRequestHostManager.cpp
 *
 *  Created on: 2015-3-12
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "HttpRequestHostManager.h"
#include <common/CheckMemoryLeak.h>

HttpRequestHostManager::HttpRequestHostManager() {
	// TODO Auto-generated constructor stub

}

HttpRequestHostManager::~HttpRequestHostManager() {
	// TODO Auto-generated destructor stub
}

string HttpRequestHostManager::GetHostByType(SiteType type) {

	string host = "";

	switch( type ) {
	case AppSite:{
		host = GetAppSite();
	}break;
	case WebSite:{
		host = GetWebSite();
	}break;
	case ChatVoiceSite:{
		host = GetChatVoiceSite();
	}break;
	case TransSite: {
		host = GetTransSite();
	}break;
	default:break;
	}

	FileLog("httprequest", "HttpRequestHostManager::GetHostByType( "
			"type : %d, "
			"host : %s "
			")",
			type,
			host.c_str()
			);

	return host;
}

void HttpRequestHostManager::SetWebSite(const string& webSite) {
	mWebSite = webSite;
}

string HttpRequestHostManager::GetWebSite() {
	return mWebSite;
}

void HttpRequestHostManager::SetAppSite(const string& appSite) {
	mAppSite = appSite;
}

string HttpRequestHostManager::GetAppSite() {
	return mAppSite;
}

void HttpRequestHostManager::SetChatVoiceSite(const string& chatVoiceSite) {
	mChatVoiceSite = chatVoiceSite;
}

string HttpRequestHostManager::GetChatVoiceSite() {
	return mChatVoiceSite;
}

void HttpRequestHostManager::SetTransSite(const string& transSite)
{
	mTransSite = transSite;
}

string HttpRequestHostManager::GetTransSite()
{
	return mTransSite;
}
