/*
 * HttpRequest.cpp
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "HttpRequest.h"

class HttpRequestRunnable : public KRunnable {
public:
	HttpRequestRunnable(HttpRequest* pHttpRequest) {
		mpHttpRequest = pHttpRequest;
	}
	~HttpRequestRunnable(){
		mpHttpRequest = NULL;
	};

protected:
	void onRun() {
		bool bFlag = mpHttpRequest->mHttpClient.Request(&mpHttpRequest->mEntiy);
		if( bFlag ) {
			if( mpHttpRequest->mpIHttpRequestCallback != NULL ) {
				mpHttpRequest->mpIHttpRequestCallback->onSuccess(mpHttpRequest->mKThread.getThreadId(),
						mpHttpRequest->mUrl,
						mpHttpRequest->mpRespondBuffer, mpHttpRequest->miCurrentSize);
			}
		} else {
			if( mpHttpRequest->mpIHttpRequestCallback != NULL ) {
				mpHttpRequest->mpIHttpRequestCallback->onFail(mpHttpRequest->mKThread.getThreadId(),
						mpHttpRequest->mUrl);
			}
		}
	}

private:
	HttpRequest *mpHttpRequest;
};

HttpRequest::HttpRequest() {
	// TODO Auto-generated constructor stub
	mpHttpRequestRunnable = new HttpRequestRunnable(this);
	mHttpClient.SetCallback(this);
	mpRespondBuffer = NULL;
	mbCache = false;
	miCurrentSize = 0;
}

HttpRequest::~HttpRequest() {
	// TODO Auto-generated destructor stub
	mHttpClient.Stop();

	if( mpHttpRequestRunnable != NULL ) {
		delete mpHttpRequestRunnable;
		mpHttpRequestRunnable = NULL;
	}

	if( mpRespondBuffer != NULL ) {
		delete[] mpRespondBuffer;
		mpRespondBuffer = NULL;
	}
}

long HttpRequest::StartRequest(string host, string path, const HttpEntiy& entiy) {
	FileLog("httprequest", "HttpRequest::StartRequest( "
			"host : %s, "
			"path : %s, "
			"entiy : %p "
			")",
			host.c_str(),
			path.c_str(),
			&entiy);

//	InitRespondBuffer();

	mHost = host;
	mPath = path;
//	mUrl = host + path;
//	mEntiy = entiy;

//	mHttpClient.Init(mUrl);
//	mKThread.stop();
//	return mKThread.start(mpHttpRequestRunnable);
	return StartRequest(host + path, entiy);
}

long HttpRequest::StartRequest(const string& url, const HttpEntiy& entiy)
{
	InitRespondBuffer();

	mUrl = url;
	mEntiy = entiy;

	mHttpClient.Init(mUrl);
	mKThread.stop();
	return mKThread.start(mpHttpRequestRunnable);
}

void HttpRequest::StopRequest(bool bWait) {
	FileLog("httprequest", "HttpRequest::StopRequest( mUrl : %s, bWait : %s )", mUrl.c_str(), bWait?"true":"false");
	mHttpClient.Stop();
	if( bWait ) {
		mKThread.stop();
	}
}
void HttpRequest::SetCallback(IHttpRequestCallback *callback){
	mpIHttpRequestCallback = callback;
}
// 设置是否缓存返回结果, 默认是缓存
void HttpRequest::SetNoCacheData(bool bCache) {
	mbCache = bCache;
}
string HttpRequest::GetUrl() const {
	return mUrl;
}
string HttpRequest::GetHost() const {
	return mHost;
}
string HttpRequest::GetPath() const {
	return mPath;
}
long HttpRequest::GetRequestId() const {
	return mKThread.getThreadId();
}
// 获取下载总数据量及已收数据字节数
void HttpRequest::GetRecvDataCount(int& total, int& recv) const {
	double dContentLength = mHttpClient.GetDownloadContentLength();
	total = (int)dContentLength;
	double dDownloadDataLength = mHttpClient.GetDownloadDataLength();
	recv = (int)dDownloadDataLength;
}
// 获取上传总数据量及已收数据字节数
void HttpRequest::GetSendDataCount(int& total, int& send) const {
	double dContentLength = mHttpClient.GetUploadContentLength();
	total = (int)dContentLength;
	double dUploadDataLength = mHttpClient.GetUploadDataLength();
	send = (int)dUploadDataLength;
}
string HttpRequest::GetContentType() const {
	FileLog("httprequest", "HttpRequest::GetHttpClient() this:%p, mHttpClient:%p, url:%s", this, &mHttpClient, mHttpClient.GetUrl().c_str());
	return mHttpClient.GetContentType();
}
void HttpRequest::onReceiveBody(HttpClient* client, const char* buf, int size) {
	// 如果不缓存, 成功返回数据为0
	if( !mbCache ) {
		AddRespondBuffer(buf, size);
	}

	if( mpIHttpRequestCallback != NULL ) {
		mpIHttpRequestCallback->onReceiveBody(
				mKThread.getThreadId(),
				mUrl,
				buf,
				size
				);
	}
}

void HttpRequest::InitRespondBuffer() {
	if( mpRespondBuffer != NULL ) {
		delete[] mpRespondBuffer;
		mpRespondBuffer = NULL;
	}

	miCurrentSize = 0;
	miCurrentCapacity = MAX_RESPONED_BUFFER;
	mpRespondBuffer = new char[miCurrentCapacity];
}

bool HttpRequest::AddRespondBuffer(const char* buf, int size) {
	bool bFlag = false;
	/* Add buffer if buffer is not enough */
	while( size + miCurrentSize >= miCurrentCapacity ) {
		miCurrentCapacity *= 2;
		bFlag = true;
	}
	if( bFlag ) {
		char *newBuffer = new char[miCurrentCapacity];
		if( mpRespondBuffer != NULL ) {
			memcpy(newBuffer, mpRespondBuffer, miCurrentSize);
			delete[] mpRespondBuffer;
			mpRespondBuffer = NULL;
		}
		mpRespondBuffer = newBuffer;
	}
	memcpy(mpRespondBuffer + miCurrentSize, buf, size);
	miCurrentSize += size;
	mpRespondBuffer[miCurrentSize] = '\0';
	return true;
}
