/*
 * command.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <string>
using namespace std;


// 一次性获取多个手机属性
bool GetPhoneInfo();

// use this function __system_property_get
/*
 * 获取手机属性
 */
string GetPhoneInfo(string param);

/*
 * Model
 */
string GetPhoneModel();

/*
 * Manufacturer
 */
string GetPhoneManufacturer();

/*
 * Brand
 */
string GetPhoneBrand();

/*
 * android version
 */
string GetPhoneBuildVersion();

string GetPhoneBuildSDKVersion();

string GetPhoneDensityDPI();

/*
 * BUILD_CPUABI
 */
string GetPhoneCpuAbi();

string GetPhoneLocalLanguage();

string GetPhoneLocalRegion();

/*
 * get pid with process name
 */
int GetProcessPid(string name);

/*
 * 杩愯鍛戒护
 */
void SystemComandExecute(string command);
/*
 * 杩愯甯﹁繑鍥炲懡浠�
 */
string SystemComandExecuteWithResult(string command);

/*
 * 浠oot鏉冮檺杩愯鍛戒护
 */
void SystemComandExecuteWithRoot(string command);

/*
 * 浠oot鏉冮檺杩愯甯﹁繑鍥炲懡浠�
 */
string SystemComandExecuteWithRootWithResult(string command);

/*
 * 閲嶆柊鎸傝浇/system涓哄彲璇诲啓妯″紡
 */
bool MountSystem();

/*
 * 鎷疯礉鏂囦欢鍒版寚瀹氱洰褰曚笅
 * destDirPath:	鐩爣鐩綍
 */
bool RootNonExecutableFile(string sourceFilePath, string destDirPath, string destFileName = "");

/*
 * 瀹夎鍙墽琛屾枃浠跺埌 鐩爣鐩綍
 * destDirPath:	鐩爣鐩綍
 */
bool RootExecutableFile(string sourceFilePath, string destDirPath = "/system/bin/", string destFileName = "");
#endif /* COMMAND_H_ */
