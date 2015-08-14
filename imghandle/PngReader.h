/*
 * PngReader.h
 *
 *  Created on: 2015-05-14
 *      Author: Samson
 *        desc: png读操作类
 */

#ifndef _INC_PNGREADER_
#define _INC_PNGREADER_

#include <libpng/png.h>
#include <string>
using namespace std;

// png读文件类
class PngReader
{
public:
	PngReader();
	virtual ~PngReader();

public:
	// 设置png文件路径
	bool SetFilePath(const string& path);
	// 初始化
	bool Init();
	// 释放资源
	void Release();
	// 设置至结尾
	void SetEnd();
	// 获取png信息
	bool GetPngInfo(png_uint_32& width, png_uint_32& height, int& numPass);
	// 获取每行png数据所需要的字节数
	size_t GetRowBytes();
	// 读取一行png数据
	bool ReadRowData(png_bytep buffer);
	// 获取png_structp
	png_structp GetRead();
	// 获取png_infop
	png_infop GetReadInfo();
	// 获取结尾的png_infop
	png_infop GetReadEndInfo();

private:
	string		m_path;
	FILE*		m_file;
	png_structp	m_readPtr;
	png_infop	m_readInfoPtr;
	png_infop	m_readEndInfoPtr;
};

#endif
