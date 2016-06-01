/*
 * DrLog.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef _INC_DRLOG_
#define _INC_DRLOG_

#include <string>
using namespace std;

#ifndef _WIN32
#include <dirent.h>
#include <sys/stat.h>
#endif

#define DLog(tag, format, ...)
#define ILog DLog
#define ELog DLog

#ifdef PRINT_JNI_LOG /* logcat */

#if	defined(FILE_JNI_LOG) /* file */
#define FileLog(fileNamePre, format, ...) KLog::LogToFile(fileNamePre, 1, NULL, format,  ## __VA_ARGS__)
//#elif defined(CONSLE_JNI_LOG) /* consle */
//#define FileLog(fileNamePre, format, ...) printf(format, ## __VA_ARGS__)
//#else
//#define FileLog(fileNamePre, format, ...) DLog(fileNamePre, format,  ## __VA_ARGS__) /* logcat杈撳嚭 */
#endif /* FILE_JNI_LOG */

#else /* no log */
#define FileLog(fileNamePre, format, ...)

#endif /* PRINT_JNI_LOG */

class KLog {
public:
	static int LogToFile(const char *fileNamePre, int level, const char *logDir, const char *fmt, ...);

	static void SetLogDirectory(string directory);
	static void SetLogEnable(bool enable);
};

// add by samson，把定义放到头文件，给外部知道
#define MAX_LOG_BUFFER 200 * 1024

#endif
