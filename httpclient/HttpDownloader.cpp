/*
 * HttpDownloader.cpp
 *
 *  Created on: 2014-12-24
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "HttpDownloader.h"
#include <common/CommonFunc.h>

#define TEMPFILENAME	".http.tmp"

HttpDownloader::HttpDownloader()
{
	m_pFile = NULL;
	m_filePath = "";
	m_callback = NULL;
	m_isDownloading = false;
}

HttpDownloader::~HttpDownloader()
{
	m_request.StopRequest(true);
	CloseTempFile();
}

// 开始下载文件
bool HttpDownloader::StartDownload(const string& url, const string& localPath, IHttpDownloaderCallback* callback)
{
	bool result = false;
	if (!url.empty() && !localPath.empty())
	{
		m_filePath = localPath;
		m_callback = callback;

		// 开始下载
		if (NULL != OpenTempFile())
		{
			HttpEntiy entiy;
			result = (HTTPREQUEST_INVALIDREQUESTID != m_request.StartRequest(url, entiy));
			m_isDownloading = result;
		}

		// 操作失败
		if (!result)
		{
			m_filePath = "";
			m_callback = NULL;
		}
	}
	return result;
}

// 停止下载文件
void HttpDownloader::Stop()
{
	m_request.StopRequest();
}

// 是否正在下载
bool HttpDownloader::IsDownloading() const
{
	return m_isDownloading;
}

// 获取下载的URL
string HttpDownloader::GetUrl() const
{
	return m_request.GetUrl();
}

// 获取本地文件路径
string HttpDownloader::GetFilePath() const
{
	return m_filePath;
}

// 获取下载进度(0-100)
void HttpDownloader::GetProgress(int& total, int& recv) const
{
	m_request.GetRecvDataCount(total, recv);
}

// 获取临时文件路径
string HttpDownloader::GetTempFilePath(const string& filePath) const
{
	return filePath + TEMPFILENAME;
}

// 打开临时文件
bool HttpDownloader::OpenTempFile()
{
	bool result = false;

	// 删除已存在的临时文件
	RemoveTempFile();

	// 打开临时文件
	string tempFilePath = GetTempFilePath(m_filePath);
	m_pFile = fopen(tempFilePath.c_str(), "a+b");
	result = (NULL != m_pFile);
	return result;
}

// 关闭临时文件
void HttpDownloader::CloseTempFile()
{
	if (NULL != m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

// 删除临时文件
void HttpDownloader::RemoveTempFile()
{
	string tempFilePath = GetTempFilePath(m_filePath);
	if (IsFileExist(tempFilePath))
	{
		RemoveFile(tempFilePath);
	}
}

// 修改临时文件名
bool HttpDownloader::RenameTempFile()
{
	// 若文件已经存在，则删除旧文件
	if (IsFileExist(m_filePath)) {
		RemoveFile(m_filePath);
	}

	// 修改文件名
	string tempFilePath = GetTempFilePath(m_filePath);
	return RenameFile(tempFilePath, m_filePath);
}

// IHttpRequestCallback
void HttpDownloader::onSuccess(long requestId, string url, const char* buf, int size)
{
	// 关闭文件
	CloseTempFile();

	// 修改文件名
	RenameTempFile();

	// 回调
	if (NULL != m_callback)
	{
		m_callback->onSuccess(this);
	}
}

void HttpDownloader::onFail(long requestId, string url)
{
	// 关闭文件
	CloseTempFile();

	// 删除临时文件
	RemoveTempFile();

	// 回调
	if (NULL != m_callback)
	{
		m_callback->onFail(this);
	}
}

void HttpDownloader::onReceiveBody(long requestId, string url, const char* buf, int size)
{
	// 写文件
	if (fwrite(buf, 1, size, m_pFile) != size)
	{
		// 写文件失败
		Stop();
	}

	// 回调
	if (NULL != m_callback)
	{
		m_callback->onUpdate(this);
	}
}
