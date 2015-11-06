/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCPhotoItem.h
 *   desc: LiveChat图片消息item
 */

#pragma once

#include <string>
#include <vector>
using namespace std;

typedef vector<string>	LCEmotionPathVector;
class LCPhotoItem
{
public:
	// 图片处理状态
	typedef enum {
		Finish,						// 已完成
		PhotoFee,					// 正在付费
		DownloadThumbFuzzyPhoto,	// 正在下载拇指不清晰图
		DownloadShowFuzzyPhoto,		// 正在下载用于显示的不清晰图
		DownloadThumbSrcPhoto,		// 正在下载拇指原图
		DownloadShowSrcPhoto,		// 正在下载用于显示的原图
		DownloadSrcPhoto,			// 正在下载原图
	} ProcessStatus;

public:
	LCPhotoItem();
	virtual ~LCPhotoItem();

public:
	// 初始化
	bool Init(const string& photoId				// 图片ID
			, const string& sendId				// 发送ID
			, const string& photoDesc			// 图片描述
			, const string& showFuzzyFilePath	// 用于显示的不清晰图路径
			, const string& thumbFuzzyFilePath	// 拇指不清晰图路径
			, const string& srcFilePath			// 原文件路径
			, const string& showSrcFilePath		// 用于显示的原图片路径
			, const string& thumbSrcFilePath	// 拇指原图路径
			, bool charge						// 是否已付费
			);

	// 设置图片处理状态
	void SetProcessStatus(LCPhotoModeType modeType, LCPhotoSizeType sizeType);

public:
	string	m_photoId;				// 图片ID
	string 	m_photoDesc;			// 图片描述
	string	m_sendId;				// 发送ID
	string	m_showFuzzyFilePath;	// 用于显示的不清晰图路径
	string 	m_thumbFuzzyFilePath;	// 拇指不清晰图路径
	string	m_srcFilePath;			// 原图路径
	string 	m_showSrcFilePath;		// 用于显示的原图路径
	string	m_thumbSrcFilePath;		// 拇指原图路径
	bool	m_charge;				// 是否已付费
	ProcessStatus	m_statusType;	// 图片处理状态
};
