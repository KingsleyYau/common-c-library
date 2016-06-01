/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCEmotionItem.h
 *   desc: LiveChat高级表情消息item
 */

#include "LCEmotionItem.h"
#include <common/CommonFunc.h>
#include <common/CheckMemoryLeak.h>

LCEmotionItem::LCEmotionItem()
{
	m_emotionId = "";
	m_imagePath = "";
	m_playBigPath = "";
}

LCEmotionItem::~LCEmotionItem()
{

}

// 初始化
bool LCEmotionItem::Init(const string& emotionId
						, const string& imagePath
						, const string& playBigPath
						, const string& playBigSubPath)
{
	bool result = false;
	if ( !emotionId.empty() )
	{
		m_emotionId = emotionId;

		if ( !imagePath.empty() )
		{
			result = true;
			if ( IsFileExist(imagePath) )
			{
				m_imagePath = imagePath;
			}

			if ( IsFileExist(playBigPath) )
			{
				m_playBigPath = playBigPath;
			}

			SetPlayBigSubPath(playBigSubPath);
		}
	}
	return result;
}

// 根据播放图片子路径枚举所有播放图片
bool LCEmotionItem::SetPlayBigSubPath(const string& playBigSubPath)
{
	bool result = false;
	if ( !playBigSubPath.empty() )
	{
		// 枚举所有播放的子图片
		LCEmotionPathVector tempPlayBigPaths;
		int i = 0;
		char tempPath[2048] = {0};
		for (i = 0; true; i++)
		{
			snprintf(tempPath, sizeof(tempPath), playBigSubPath.c_str(), i);
			if ( IsFileExist(tempPath) )
			{
				tempPlayBigPaths.push_back(tempPath);
			}
			else
			{
				break;
			}
		}

		// 若不为空则赋值
		if ( !tempPlayBigPaths.empty() )
		{
			m_playBigPaths = tempPlayBigPaths;

			result = true;
		}
	}
	return result;
}
