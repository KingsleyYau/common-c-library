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

#include <sys/system_properties.h>

#include "KLog.h"

#define PRODUCT_MODEL		"ro.product.model"	 			// Model
#define PRODUCT_BRAND		"ro.product.brand"	 			// Brand
#define PRODUCT_MANUFACT	"ro.product.manufacturer"		// Manufacturer
#define BUILD_VERSION		"ro.build.version.release"		// android release
#define BUILD_SDK			"ro.build.version.sdk"			// android sdk version
#define BUILD_CPUABI		"ro.product.cpu.abi"			// CPU info

#define SF_DENSITY			"ro.sf.lcd_density"				// density(DPI)

#define LOCAL_LANGUAGE		"ro.product.locale.language"	// language
#define LOCAL_REGION		"ro.product.locale.region"		// country

// use this function __system_property_get
/*
 * 获取手机属性
 */
static inline string GetPhoneInfo(string param) {

	string findName = " ";
	string sCommand = "cat /system/build.prop 2>&1";
	string model = "";

	if(param.length() == 0) {
		return model;
	}

	FILE *ptr = popen(sCommand.c_str(), "r");
	if(ptr != NULL) {
		char buffer[2048] = {'\0'};

		while(fgets(buffer, 2048, ptr) != NULL) {
			string reslut = buffer;
			if(string::npos != reslut.find(param)) {
				// +1 for =
				model = reslut.substr(param.length() + 1, reslut.length() - (param.length() + 1 + 1));
				break;
			}
		}
		pclose(ptr);
		ptr = NULL;
	}

//	char temp[1024];
//	memset(temp, '\0', sizeof(temp));
//	__system_property_get(param.c_str(), temp);
//	model = temp;

	return model;
}

/*
 * Model
 */
static inline string GetPhoneModel() {
	string model = GetPhoneInfo(PRODUCT_MODEL);
	if(model.length() > 0) {
		DLog("jni.command::GetPhoneModel()", "Model : %s", model.c_str());
	}
	return model;
}

/*
 * Manufacturer
 */
static inline string GetPhoneManufacturer() {
	string strManufacturer = GetPhoneInfo(PRODUCT_MANUFACT);
	if (!strManufacturer.empty()) {
		DLog("jni.command::GetPhoneManufacturer()", "Manufacturer : %s", strManufacturer.c_str());
	}
	return strManufacturer;
}

/*
 * Brand
 */
static inline string GetPhoneBrand() {
	string model = GetPhoneInfo(PRODUCT_BRAND);
	if(model.length() > 0) {
		DLog("jni.command::GetPhoneBrand()", "Brand : %s", model.c_str());
	}
	return model;
}

/*
 * 鑾峰彇璁惧 android version
 */
static inline string GetPhoneBuildVersion() {
	string model = GetPhoneInfo(BUILD_VERSION);
	if(model.length() > 0) {
		DLog("jni.command::GetPhoneBuildVersion()", "鎵惧埌鎵嬫満Android鐗堟湰:%s", model.c_str());
	}
	return model;
}

static inline string GetPhoneBuildSDKVersion() {
	string strSdkVersion = GetPhoneInfo(BUILD_SDK);
	if (!strSdkVersion.empty()) {
		DLog("jni.command::GetPhoneBuildSDKVersion()", "SDK Version:%s", strSdkVersion.c_str());
	}
	return strSdkVersion;
}

static inline string GetPhoneDensityDPI() {
	string strDensityDPI = GetPhoneInfo(SF_DENSITY);
	if (!strDensityDPI.empty()) {
		DLog("jni.command::GetPhoneDensityDPI()", "Density DPI:%s", strDensityDPI.c_str());
	}
	return strDensityDPI;
}

/*
 * 鑾峰彇璁惧 BUILD_CPUABI
 */
static inline string GetPhoneCpuAbi() {
	string model = GetPhoneInfo(BUILD_CPUABI);
	if(model.length() > 0) {
		DLog("jni.command::GetPhoneCpuAbi()", "Android CPU ABI : %s", model.c_str());
	}
	return model;
}

static inline string GetPhoneLocalLanguage() {
	string strLanguage = GetPhoneInfo(LOCAL_LANGUAGE);
	if (!strLanguage.empty()) {
		DLog("jni.command::GetPhoneLocalLanguage()", "Language:%s", strLanguage.c_str());
	}
	return strLanguage;
}

static inline string GetPhoneLocalRegion() {
	string strRegion = GetPhoneInfo(LOCAL_REGION);
	if (!strRegion.empty()) {
		DLog("jni.command::GetPhoneLocalRegion()", "Region:%s", strRegion.c_str());
	}
	return strRegion;
}

/*
 * 鑾峰彇杩涚▼鍚嶇殑Pid
 */
static inline int GetProcessPid(string name) {
	DLog("jni.command::GetProcessPid", "姝ｅ湪鑾峰彇杩涚▼(%s)Pid...", name.c_str());
	int iPid = -1;

	string findName = " ";
	findName += name;
	string sCommand = "ps 2>&1";
	string reslut = "";

	FILE *ptr = popen(sCommand.c_str(), "r");
	if(ptr != NULL) {
		char buffer[2048] = {'\0'};

		// 鑾峰彇绗竴琛屽瓧娈�
		int iColumn = -1, index = 0;
		if(fgets(buffer, 2048, ptr) != NULL) {
			char *p = strtok(buffer, " ");
			while(p != NULL) {
				reslut = p;
				if(reslut == "PID") {
					iColumn = index;
					break;
				}
				index++;
				p = strtok(NULL, " ");
			}
		}

		// 鑾峰彇杩涚▼pid
		if(iColumn != -1) {
			while(fgets(buffer, 2048, ptr) != NULL) {
				string reslut = buffer;
				if(string::npos != reslut.find(findName.c_str())) {
					char *p = strtok(buffer, " ");
					for(int i = 0; p != NULL; i++) {
						if(i == iColumn) {
							// 鎵惧埌杩涚▼pid
							iPid = atoi(p);
							DLog("jni.command::GetProcessPid", "鎵惧埌杩涚▼Pid:%s", p);
							break;
						}
						p = strtok(NULL, " ");
					}
				}
			}
		}

		pclose(ptr);
		ptr = NULL;
	}

	return iPid;
}

/*
 * 杩愯鍛戒护
 */
static inline void SystemComandExecute(string command) {
	string sCommand = command;
	sCommand += " &>/dev/null";
	system(sCommand.c_str());
	DLog("jni.command::SystemComandExecute", "command:%s", sCommand.c_str());
}

/*
 * 杩愯甯﹁繑鍥炲懡浠�
 */
static inline string SystemComandExecuteWithResult(string command) {
	string result = "";
	string sCommand = command;
	sCommand += " 2>&1";

	FILE *ptr = popen(sCommand.c_str(), "r");
	if(ptr != NULL) {
		char buffer[2048] = {'\0'};
		while(fgets(buffer, 2048, ptr) != NULL) {
			result += buffer;
		}
		pclose(ptr);
		ptr = NULL;
	}
	DLog("jni.command::SystemComandExecuteWithResult", "command:%s \ncommand result:%s", sCommand.c_str(), result.c_str());
	return result;
}

/*
 * 浠oot鏉冮檺杩愯鍛戒护
 */
static inline void SystemComandExecuteWithRoot(string command) {
	string sCommand;
	sCommand = "echo \"";//"su -c \"";
	sCommand += command;
	sCommand += "\"|su";
	SystemComandExecute(sCommand);
}

/*
 * 浠oot鏉冮檺杩愯甯﹁繑鍥炲懡浠�
 */
static inline string SystemComandExecuteWithRootWithResult(string command) {
	string sCommand = command;
	sCommand = "echo \"";//"su -c \"";
	sCommand += command;
	sCommand += "\"|su";
	return SystemComandExecuteWithResult(sCommand);
}

/*
 * 閲嶆柊鎸傝浇/system涓哄彲璇诲啓妯″紡
 */
static inline bool MountSystem() {
	bool bFlag = false;

	char pBuffer[2048] = {'\0'};
	string result = "";

	// 鑾峰彇鍘�system鎸傝浇
	FILE *ptr = popen("mount", "r");
	if(ptr != NULL) {

		while(fgets(pBuffer, 2048, ptr) != NULL) {
			result = pBuffer;

			if(string::npos != result.find("/system")) {
				// 鎵惧埌/system鎸傝浇璺緞
				char* dev = strtok(pBuffer, " ");
				result = dev;
				DLog("jni.command::MountSystem", "鎵惧埌/system鎸傝浇璺緞:%s", result.c_str());
				break;
			}
		}
		pclose(ptr);
		ptr = NULL;
	}

	// 鏇存敼鎸傝浇涓簉w
	sprintf(pBuffer, "mount -o remount rw,%s /system", result.c_str());
	result = SystemComandExecuteWithRootWithResult(pBuffer);
	if(result.length() == 0) {
		DLog("jni.command::MountSystem", "鎸傝浇/system涓哄彲璇诲啓鎴愬姛!");
		bFlag = true;
	}
	else {
		ELog("jni.command::MountSystem", "鎸傝浇/system涓哄彲璇诲啓澶辫触!");
	}

	return bFlag;
}

/*
 * 鎷疯礉鏂囦欢鍒版寚瀹氱洰褰曚笅
 * destDirPath:	鐩爣鐩綍
 */
static inline bool RootNonExecutableFile(string sourceFilePath, string destDirPath, string destFileName = "") {
	bool bFlag = false;

	char pBuffer[2048] = {'\0'};
	string result = "";

	if(MountSystem()) {
		// 寮�鎷疯礉
		string fileName = sourceFilePath;

		if(destFileName.length() == 0) {
			// 娌℃湁鎸囧畾鏂囦欢鍚�鐢ㄥ師鏉ョ殑鍚嶅瓧
			string::size_type pos = string::npos;
			pos = sourceFilePath.find_last_of('/');
			if(string::npos != pos) {
				pos ++;
				fileName = sourceFilePath.substr(pos, sourceFilePath.length() - pos);
			}
			fileName = destDirPath + fileName;
		}
		else {
			// 鎸囧畾鏂囦欢鍚�
			fileName = destDirPath + destFileName;
		}

		sprintf(pBuffer, "cat %s > %s", sourceFilePath.c_str(), fileName.c_str());
		result = SystemComandExecuteWithRootWithResult(pBuffer);
		if(result.length() == 0) {
			// 鎷疯礉鎴愬姛
			DLog("jni.command::RootNonExecutableFile", "鎷疯礉%s鍒�s鎴愬姛%s!", sourceFilePath.c_str(), fileName.c_str());
			bFlag = true;
		}
		else {
			ELog("jni.command::RootNonExecutableFile", "鎷疯礉%s鍒�s澶辫触%s!", sourceFilePath.c_str(), fileName.c_str());
		}
	}

	return bFlag;
}

/*
 * 瀹夎鍙墽琛屾枃浠跺埌 鐩爣鐩綍
 * destDirPath:	鐩爣鐩綍
 */
static inline bool RootExecutableFile(string sourceFilePath, string destDirPath = "/system/bin/", string destFileName = "") {
	bool bFlag = false;

	char pBuffer[2048] = {'\0'};
	string result = "";

	// 寮�鎷疯礉
	string fileName = "";

	if(destFileName.length() == 0) {
		// 娌℃湁鎸囧畾鏂囦欢鍚�鐢ㄥ師鏉ョ殑鍚嶅瓧
		string::size_type pos = string::npos;
		pos = sourceFilePath.find_last_of('/');
		if(string::npos != pos) {
			pos ++;
			fileName = sourceFilePath.substr(pos, sourceFilePath.length() - pos);
		}
	}
	else {
		// 鎸囧畾鏂囦欢鍚�
		fileName = destFileName;
	}

	// 濡傛灉鍦ㄨ繍琛屽厛鍏抽棴
	int iPid = GetProcessPid(fileName);
	if(iPid != -1) {
		DLog("jni.command::RootExecutableFile", "鍙戠幇%s(PID:%d)姝ｅ湪杩愯, 鍏堟潃鎺�", fileName.c_str(), iPid);
		sprintf(pBuffer, "kill -9 %d", iPid);
		SystemComandExecuteWithRoot(pBuffer);
	}

	if(RootNonExecutableFile(sourceFilePath, destDirPath, fileName)) {
		fileName = destDirPath + fileName;
		sprintf(pBuffer, "chmod 4755 %s", fileName.c_str());
		result = SystemComandExecuteWithRootWithResult(pBuffer);
		if(result.length() == 0) {
			// 鏇存敼鏉冮檺鎴愬姛
			DLog("jni.command::RootExecutableFile", "鎻愬崌%s鏉冮檺涓�755鎴愬姛!", fileName.c_str());
			bFlag = true;
		}
		else {
			ELog("jni.command::RootExecutableFile", "鎻愬崌%s鏉冮檺涓�755澶辫触!", fileName.c_str());
		}
	}

	return bFlag;
}
#endif /* COMMAND_H_ */
