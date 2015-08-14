/*
 * author: Samson.Fan
 *   date: 2015-03-31
 *   file: CommonDef.h
 *   desc: 跨平台的公共定义
 */

#pragma once

#ifdef WIN32
	// include 头文件
	#include <stdio.h>
	#include <time.h>
	
	// define
	#define snprintf sprintf_s

	// function
	int gettimeofday(struct timeval *tp, void *tzp)
	{
		time_t clock;
		struct tm tm;
		SYSTEMTIME wtm;

		GetLocalTime(&wtm);
		tm.tm_year     = wtm.wYear - 1900;
		tm.tm_mon     = wtm.wMonth - 1;
		tm.tm_mday     = wtm.wDay;
		tm.tm_hour     = wtm.wHour;
		tm.tm_min     = wtm.wMinute;
		tm.tm_sec     = wtm.wSecond;
		tm. tm_isdst    = -1;
		clock = mktime(&tm);
		tp->tv_sec = clock;
		tp->tv_usec = wtm.wMilliseconds * 1000;

		return (0);
	}
#else
	// include 头文件
	#include <stdio.h>
	#include <sys/time.h>
	#include <unistd.h>

	// define
	#define Sleep(ms)  usleep(ms * 1000)

#endif

inline long getCurrentTime()
{
	long result = 0;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	result =  tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return result;
}

inline long DiffTime(long start, long end)
{
	return (end > start ? end - start : (unsigned long)-1 - end + start);
}
