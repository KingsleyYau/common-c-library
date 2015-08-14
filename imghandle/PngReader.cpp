/*
 * PngReader.cpp
 *
 *  Created on: 2015-05-14
 *      Author: Samson
 *        desc: png读操作类
 */

#include "PngReader.h"
#include <common/KLog.h>

PngReader::PngReader()
{
	m_file = NULL;
	m_readPtr = NULL;
	m_readInfoPtr = NULL;
	m_readEndInfoPtr = NULL;
}

PngReader::~PngReader()
{
	Release();
}

// 设置png文件路径
bool PngReader::SetFilePath(const string& path)
{
	bool result = false;
	if (!path.empty())
	{
		m_path = path;
		result = true;
	}
	return result;
}

// 初始化
bool PngReader::Init()
{
	Release();

	bool result = false;

	do {
		png_uint_32 i = 0;

		// 打开源文件
		m_file = fopen(m_path.c_str(), "rb");
		if (m_file == NULL) {
			FileLog("ImageHandler", "ConvertEmotionImage() open file fail! path:%s", m_path.c_str());
			break;
		}

		// 初始化libpng变量
		m_readPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (m_readPtr == NULL)
		{
			FileLog("ImageHandler", "ConvertEmotionImage() png_create_read_struct() read fail!");
			break;
		}

		m_readInfoPtr = png_create_info_struct(m_readPtr);
		if (m_readInfoPtr == NULL)
		{
			FileLog("ImageHandler", "ConvertEmotionImage() png_create_info_struct() read info fail!");
			break;
		}

		m_readEndInfoPtr = png_create_info_struct(m_readPtr);
		if (m_readEndInfoPtr == NULL)
		{
			FileLog("ImageHandler", "ConvertEmotionImage() png_create_info_struct() read end info fail!");
			break;
		}

		if (setjmp(png_jmpbuf(m_readPtr)))
		{
			break;
		}

		png_init_io(m_readPtr, m_file);
		png_read_info(m_readPtr, m_readInfoPtr);

		result = true;
	} while (0);

	if (!result)
	{
		Release();
	}

	return result;
}

// 释放资源
void PngReader::Release()
{
	if (NULL != m_readPtr)
	{
		png_destroy_read_struct(&m_readPtr, &m_readInfoPtr, &m_readEndInfoPtr);
		m_readPtr = NULL;
		m_readInfoPtr = NULL;
		m_readEndInfoPtr = NULL;
	}

	if (NULL != m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

// 设置至结尾
void PngReader::SetEnd()
{
	if (NULL != m_readPtr && NULL != m_readEndInfoPtr) {
		png_read_end(m_readPtr, m_readEndInfoPtr);
	}
}

// 获取png信息
bool PngReader::GetPngInfo(png_uint_32& width, png_uint_32& height, int& numPass)
{
	bool result = false;
	if (NULL != m_readPtr
		&& NULL != m_readInfoPtr)
	{
		int bitDepth;
		int colorType;
		int interlaceType;
		int compressionType;
		int filterType;
		if ( png_get_IHDR(m_readPtr, m_readInfoPtr, &width, &height
			  , &bitDepth, &colorType
			  , &interlaceType, &compressionType
			  , &filterType) != 0 )
		{
			if (PNG_INTERLACE_NONE == interlaceType)
			{
				numPass = 1;
				result = true;
			}
			else if (PNG_INTERLACE_ADAM7 == interlaceType)
			{
				numPass = 7;
				result = true;
			}
		}
	}
	return result;
}

// 获取每行png数据所需要的字节数
size_t PngReader::GetRowBytes()
{
	size_t bytes = 0;
	if (NULL != m_readPtr
		&& NULL != m_readInfoPtr)
	{
		bytes = png_get_rowbytes(m_readPtr, m_readInfoPtr);
	}
	return bytes;
}

// 读取一行png数据
bool PngReader::ReadRowData(png_bytep buffer)
{
	bool result = false;
	if (NULL != m_readPtr
		&& NULL != m_readInfoPtr)
	{
		png_read_row(m_readPtr, buffer, NULL);
		result = true;
	}
	return result;
}

// 获取png_structp
png_structp PngReader::GetRead()
{
	return m_readPtr;
}

// 获取png_infop
png_infop PngReader::GetReadInfo()
{
	return m_readInfoPtr;
}

// 获取结尾的png_infop
png_infop PngReader::GetReadEndInfo()
{
	return m_readEndInfoPtr;
}
