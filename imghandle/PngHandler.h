/*
 * PngHandler.h
 *
 *  Created on: 2015-05-12
 *      Author: Samson
 */

#ifndef _INC_PNGHANDLER_
#define _INC_PNGHANDLER_

#include <string>
using namespace std;

class PngHandler
{
public:
	PngHandler() {}
	virtual ~PngHandler() {}
public:
	// 把一张很长的图片裁剪成一张张小图片
	static bool ConvertImage(const string& path);
};

#endif
