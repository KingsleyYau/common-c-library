/*
 * command.cpp
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "command.h"
#include <sys/system_properties.h>
#include "KLog.h"

#define PRODUCT_MODEL		"ro.product.model"	 			// Model
#define PRODUCT_BRAND		"ro.product.brand"	 			// Brand
#define PRODUCT_MANUFACT	"ro.product.manufacturer"		// Manufacturer
#define BUILD_VERSION		"ro.build.version.release"		// android release
#define BUILD_SDK			"ro.build.version.sdk"			// android sdk version
#define BUILD_CPUABI		"ro.product.cpu.abi"			// CPU info
#define BUILD_CPUABILIST	"ro.product.cpu.abilist"		// CPU abi list
#define SF_DENSITY			"ro.sf.lcd_density"				// density(DPI)
#define LOCAL_LANGUAGE		"ro.product.locale.language"	// language
#define LOCAL_REGION		"ro.product.locale.region"		// country
#define BOOT_SERIALNO		"ro.boot.serialno"				// boot serialno
#define RO_SERIALNO			"ro.serialno"					// serialno
#define RO_HARDWARE			"ro.hardware"					// hardware
#define DISPLAY_ID			"ro.build.display.id"			// displayid
#define VER_INCREMENTAL		"ro.build.version.incremental"	// version incremental
#define BUILD_HOST			"ro.build.host"					// host
#define BUILD_FLAVOR		"ro.build.flavor"				// flavor
#define PRODUCT_NAME		"ro.product.name"				// product name
#define PRODUCT_DEVICE		"ro.product.device"				// product device
#define BOARD_PLATFORM		"ro.board.platform"				// platform
#define RO_CHIPNAME			"ro.chipname"					// chipname
#define BUILD_DESCRIPTION	"ro.build.description"			// description

typedef struct PhoneInfo {
	string model;
	string manufacturer;
	string brand;
	string buildVersion;
	string buildSDKVersion;
	string densityDpi;
	string cpuAbi;
	string cpuAbiList;
	string language;
	string region;
	string bootSerialno;
	string serialno;
	string hardware;
	string displayId;
	string verIncremental;
	string host;
	string flavor;
	string productName;
	string productDevice;
	string platform;
	string chipname;
	string description;
} PHONEINFO, *LPPHONEINFO;
static PHONEINFO g_phoneInfo;

// 一次性获取多个手机属性
bool GetPhoneInfo()
{
	bool result = false;
	string sCommand = "cat /system/build.prop 2>&1";

	FILE *ptr = popen(sCommand.c_str(), "r");
	if(ptr != NULL) {
		result = true;
		const unsigned int bufferLen = 2048;
		char buffer[bufferLen] = {0};

		while(fgets(buffer, bufferLen, ptr) != NULL) {
			string result = buffer;

			// skip if start with #
			if( result.length() > 0 && result.c_str()[0] == '#' ) {
				continue;
			}

			// skip if not found '='
			std::string::size_type sep = result.find("=");
			if( string::npos == sep ) {
				continue;
			}

			string key = result.substr(0, sep);

			if( key == PRODUCT_MODEL )
			{
				g_phoneInfo.model = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if( key == PRODUCT_MANUFACT )
			{
				g_phoneInfo.manufacturer = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if( key == PRODUCT_BRAND )
			{
				g_phoneInfo.brand = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if( key == BUILD_VERSION )
			{
				g_phoneInfo.buildVersion = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if( key == BUILD_SDK )
			{
				g_phoneInfo.buildSDKVersion = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if( key == SF_DENSITY )
			{
				g_phoneInfo.densityDpi = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if( key == BUILD_CPUABI )
			{
				g_phoneInfo.cpuAbi = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if( key == BUILD_CPUABILIST )
			{
				g_phoneInfo.cpuAbiList = result.substr(key.length() + 1, result. length() - (key.length() + 1 + 1));
			}
			else if( key == LOCAL_LANGUAGE )
			{
				g_phoneInfo.language = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if( key == LOCAL_REGION )
			{
				g_phoneInfo.region = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if ( key == BOOT_SERIALNO )
			{
				g_phoneInfo.bootSerialno = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if ( key == RO_SERIALNO )
			{
				g_phoneInfo.serialno = result.substr(key.length() + 1, result.length() - (key.length() + 1 + 1));
			}
			else if ( key == RO_HARDWARE )
			{
				g_phoneInfo.hardware = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == DISPLAY_ID )
			{
				g_phoneInfo.displayId = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == VER_INCREMENTAL )
			{
				g_phoneInfo.verIncremental = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == BUILD_HOST )
			{
				g_phoneInfo.host = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == BUILD_FLAVOR )
			{
				g_phoneInfo.flavor = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == PRODUCT_NAME )
			{
				g_phoneInfo.productName = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == PRODUCT_DEVICE )
			{
				g_phoneInfo.productDevice = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == BOARD_PLATFORM )
			{
				g_phoneInfo.platform = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == RO_CHIPNAME )
			{
				g_phoneInfo.chipname = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
			else if ( key == BUILD_DESCRIPTION )
			{
				g_phoneInfo.description = result.substr(key.length() + 1, result.length() - (key.length() +1 + 1));
			}
		}
		pclose(ptr);
		ptr = NULL;
	}

//	char temp[1024];
//	memset(temp, '\0', sizeof(temp));
//	__system_property_get(param.c_str(), temp);
//	model = temp;
	return result;
}

// use this function __system_property_get
/*
 * 获取手机属性
 */
string GetPhoneInfo(string param) {

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
			string result = buffer;
			if(string::npos != result.find(param)) {
				// +1 for =
				model = result.substr(param.length() + 1, result.length() - (param.length() + 1 + 1));
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
string GetPhoneModel() {
//	string model = GetPhoneInfo(PRODUCT_MODEL);
//	if(model.length() > 0) {
//		DLog("jni.command::GetPhoneModel()", "Model : %s", model.c_str());
//	}
//	return model;
	return g_phoneInfo.model;
}

/*
 * Manufacturer
 */
string GetPhoneManufacturer() {
//	string strManufacturer = GetPhoneInfo(PRODUCT_MANUFACT);
//	if (!strManufacturer.empty()) {
//		DLog("jni.command::GetPhoneManufacturer()", "Manufacturer : %s", strManufacturer.c_str());
//	}
//	return strManufacturer;
	return g_phoneInfo.manufacturer;
}

/*
 * Brand
 */
string GetPhoneBrand() {
//	string model = GetPhoneInfo(PRODUCT_BRAND);
//	if(model.length() > 0) {
//		DLog("jni.command::GetPhoneBrand()", "Brand : %s", model.c_str());
//	}
//	return model;
	return g_phoneInfo.brand;
}

/*
 * android version
 */
string GetPhoneBuildVersion() {
//	string model = GetPhoneInfo(BUILD_VERSION);
//	if(model.length() > 0) {
//		DLog("jni.command::GetPhoneBuildVersion()", "鎵惧埌鎵嬫満Android鐗堟湰:%s", model.c_str());
//	}
//	return model;
	return g_phoneInfo.buildVersion;
}

string GetPhoneBuildSDKVersion() {
//	string strSdkVersion = GetPhoneInfo(BUILD_SDK);
//	if (!strSdkVersion.empty()) {
//		DLog("jni.command::GetPhoneBuildSDKVersion()", "SDK Version:%s", strSdkVersion.c_str());
//	}
//	return strSdkVersion;
	return g_phoneInfo.buildSDKVersion;
}

string GetPhoneDensityDPI() {
//	string strDensityDPI = GetPhoneInfo(SF_DENSITY);
//	if (!strDensityDPI.empty()) {
//		DLog("jni.command::GetPhoneDensityDPI()", "Density DPI:%s", strDensityDPI.c_str());
//	}
//	return strDensityDPI;
	return g_phoneInfo.densityDpi;
}

/*
 * BUILD_CPUABI
 */
string GetPhoneCpuAbi() {
//	string model = GetPhoneInfo(BUILD_CPUABI);
//	if(model.length() > 0) {
//		DLog("jni.command::GetPhoneCpuAbi()", "Android CPU ABI : %s", model.c_str());
//	}
//	return model;
	return g_phoneInfo.cpuAbi;
}

string GetPhoneCpuAbiList()
{
	return g_phoneInfo.cpuAbiList;
}

string GetPhoneLocalLanguage() {
//	string strLanguage = GetPhoneInfo(LOCAL_LANGUAGE);
//	if (!strLanguage.empty()) {
//		DLog("jni.command::GetPhoneLocalLanguage()", "Language:%s", strLanguage.c_str());
//	}
//	return strLanguage;
	return g_phoneInfo.language;
}

string GetPhoneLocalRegion() {
//	string strRegion = GetPhoneInfo(LOCAL_REGION);
//	if (!strRegion.empty()) {
//		DLog("jni.command::GetPhoneLocalRegion()", "Region:%s", strRegion.c_str());
//	}
//	return strRegion;
	return g_phoneInfo.region;
}

string GetPhoneHardware()
{
	return g_phoneInfo.hardware;
}

string GetPhoneDisplayId()
{
	return g_phoneInfo.displayId;
}

string GetPhoneVersionIncremental()
{
	return g_phoneInfo.verIncremental;
}

string GetPhoneHost()
{
	return g_phoneInfo.host;
}

string GetPhoneFlavor()
{
	return g_phoneInfo.flavor;
}

string GetPhoneProductName()
{
	return g_phoneInfo.productName;
}

string GetPhoneProductDevice()
{
	return g_phoneInfo.productDevice;
}

string GetPhoneBoardPlatform()
{
	return g_phoneInfo.platform;
}

string GetPhoneChipName()
{
	return g_phoneInfo.chipname;
}

string GetPhoneBuildDescription()
{
	return g_phoneInfo.description;
}

/*
 * get pid with process name
 */
int GetProcessPid(string name) {
	DLog("jni.command::GetProcessPid", "姝ｅ湪鑾峰彇杩涚▼(%s)Pid...", name.c_str());
	int iPid = -1;

	string findName = " ";
	findName += name;
	string sCommand = "ps 2>&1";
	string result = "";

	FILE *ptr = popen(sCommand.c_str(), "r");
	if(ptr != NULL) {
		char buffer[2048] = {'\0'};

		// 鑾峰彇绗竴琛屽瓧娈�
		int iColumn = -1, index = 0;
		if(fgets(buffer, 2048, ptr) != NULL) {
			char *p = strtok(buffer, " ");
			while(p != NULL) {
				result = p;
				if(result == "PID") {
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
				string result = buffer;
				if(string::npos != result.find(findName.c_str())) {
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
void SystemComandExecute(string command) {
	string sCommand = command;
	sCommand += " &>/dev/null";
	system(sCommand.c_str());
	DLog("jni.command::SystemComandExecute", "command:%s", sCommand.c_str());
}

/*
 * 杩愯甯﹁繑鍥炲懡浠�
 */
string SystemComandExecuteWithResult(string command) {
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
void SystemComandExecuteWithRoot(string command) {
	string sCommand;
	sCommand = "echo \"";//"su -c \"";
	sCommand += command;
	sCommand += "\"|su";
	SystemComandExecute(sCommand);
}

/*
 * 浠oot鏉冮檺杩愯甯﹁繑鍥炲懡浠�
 */
string SystemComandExecuteWithRootWithResult(string command) {
	string sCommand = command;
	sCommand = "echo \"";//"su -c \"";
	sCommand += command;
	sCommand += "\"|su";
	return SystemComandExecuteWithResult(sCommand);
}

/*
 * 閲嶆柊鎸傝浇/system涓哄彲璇诲啓妯″紡
 */
bool MountSystem() {
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
bool RootNonExecutableFile(string sourceFilePath, string destDirPath, string destFileName) {
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
bool RootExecutableFile(string sourceFilePath, string destDirPath, string destFileName) {
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
