/*
 * RequestProfileController.cpp
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "RequestProfileController.h"

RequestProfileController::RequestProfileController(HttpRequestManager *pHttpRequestManager, RequestProfileControllerCallback callback/* CallbackManager* pCallbackManager*/) {
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(pHttpRequestManager);
	mRequestProfileControllerCallback = callback;
}

RequestProfileController::~RequestProfileController() {
	// TODO Auto-generated destructor stub
}

/* IHttpRequestManagerCallback */
void RequestProfileController::onSuccess(long requestId, string url, const char* buf, int size) {
	FileLog("httprequest", "RequestProfileController::onSuccess( url : %s, buf( size : %d ) )", url.c_str(), size);
	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestProfileController::onSuccess(), buf: %s", buf);
	}

	/* parse base result */
	string errnum = "";
	string errmsg = "";
	Json::Value data;

	bool bFlag = HandleResult(buf, size, errnum, errmsg, &data);

	/* resopned parse ok, callback success */
	if( url.compare(GetMyProfilePath) == 0 ) {
		/*  2.1.查询个人信息 */
		ProfileItem item;
		item.Parse(data);
		if( mRequestProfileControllerCallback.onGetMyProfile != NULL ) {
			mRequestProfileControllerCallback.onGetMyProfile(requestId, bFlag, item, errnum, errmsg);
		}
	} else if( url.compare(UpdateMyProfilePath) == 0 ) {
		/* 2.2.修改个人信息 */
		bool rsModified = false;
		if( data.isObject() && data[PROFILE_JJ_RESULT].isInt() ) {
			rsModified = (data[PROFILE_JJ_RESULT].asInt() == 1)?true:false;
		}
		if( mRequestProfileControllerCallback.onUpdateMyProfile != NULL ) {
			mRequestProfileControllerCallback.onUpdateMyProfile(requestId, bFlag, rsModified,
					errnum, errmsg);
		}
	} else if( url.compare(StartEditMyProfilePath) == 0 ) {
		/* 2.3.开始编辑简介触发计时 */
		if( mRequestProfileControllerCallback.onStartEditResume != NULL ) {
			mRequestProfileControllerCallback.onStartEditResume(requestId, bFlag, errnum, errmsg);
		}
	} else if( url.compare(SaveContactPath) == 0 ) {
		/* 2.4.保存联系电话 */
		if( mRequestProfileControllerCallback.onSaveContact != NULL ) {
			mRequestProfileControllerCallback.onSaveContact(requestId, bFlag, errnum, errmsg);
		}
	} else if( url.compare(UploadPhotoPath) == 0 ) {
		/* 2.5.上传头像 */
		if( mRequestProfileControllerCallback.onUploadHeaderPhoto != NULL ) {
			mRequestProfileControllerCallback.onUploadHeaderPhoto(requestId, bFlag, errnum, errmsg);
		}
	}
	FileLog("httprequest", "RequestProfileController::onSuccess() end, url:%s", url.c_str());
}
void RequestProfileController::onFail(long requestId, string url) {
	FileLog("httprequest", "RequestProfileController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(GetMyProfilePath) == 0 ) {
		/* 2.1.查询个人信息 */
		if( mRequestProfileControllerCallback.onGetMyProfile != NULL ) {
			ProfileItem item;
			mRequestProfileControllerCallback.onGetMyProfile(requestId, false, item,
					LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(UpdateMyProfilePath) == 0 ) {
		/* 2.2.修改个人信息 */
		if( mRequestProfileControllerCallback.onUpdateMyProfile != NULL ) {
			mRequestProfileControllerCallback.onUpdateMyProfile(requestId, false, false,
					LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(StartEditMyProfilePath) == 0 ) {
		/* 2.3.开始编辑简介触发计时 */
		if( mRequestProfileControllerCallback.onStartEditResume != NULL ) {
			mRequestProfileControllerCallback.onStartEditResume(requestId, false,
					LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(SaveContactPath) == 0 ) {
		/* 2.4.保存联系电话 */
		if( mRequestProfileControllerCallback.onSaveContact != NULL ) {
			mRequestProfileControllerCallback.onSaveContact(requestId, false,
					LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(UploadPhotoPath) == 0 ) {
		/* 2.5.上传头像 */
		if( mRequestProfileControllerCallback.onUploadHeaderPhoto != NULL ) {
			mRequestProfileControllerCallback.onUploadHeaderPhoto(requestId, false,
					LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	}
	FileLog("httprequest", "RequestProfileController::onFail() end, url:%s", url.c_str());
}

/**
 * 2.1.查询个人信息
 * @return				请求唯一标识
 */
long RequestProfileController::GetMyProfile() {
	HttpEntiy entiy;

	string url = GetMyProfilePath;
	FileLog("httprequest", "RequestProfileController::GetMyProfile( "
			"url : %s "
			")",
			url.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 2.2.修改个人信息
 * @param weight		体重
 * @param height		身高
 * @param language		语言
 * @param ethnicity		人种
 * @param religion		宗教
 * @param education		教育程度
 * @param profession	职业
 * @param income		收入情况
 * @param children		子女状况
 * @param smoke			吸烟情况
 * @param drink			喝酒情况
 * @param resume		drink
 * @return				请求唯一标识
 */
long RequestProfileController::UpdateProfile(int weight, int height, int language, int ethnicity,
		int religion, int education, int profession, int income, int children, int smoke, int drink,
		string resume, list<string> interests) {
	HttpEntiy entiy;
	char temp[16];

	if( weight > -1 ) {
		sprintf(temp, "%d", CodeToWeight(weight));
		entiy.AddContent(PROFILE_WEIGHT, temp);
	}

	if( height > -1 ) {
		sprintf(temp, "%d", CodeToHeight(height));
		entiy.AddContent(PROFILE_HEIGHT, temp);
	}

	if( language > -1 ) {
		sprintf(temp, "%d", language);
		entiy.AddContent(PROFILE_LANGUAGE, temp);
	}

	if( ethnicity > -1 ) {
		sprintf(temp, "%d", CodeToEthnicity(ethnicity));
		entiy.AddContent(PROFILE_ETHNICITY, temp);
	}

	if( religion > -1 ) {
		sprintf(temp, "%d", religion);
		entiy.AddContent(PROFILE_RELIGION, temp);
	}

	if( education > -1 ) {
		sprintf(temp, "%d", education);
		entiy.AddContent(PROFILE_EDUCATION, temp);
	}

	if( profession > -1 ) {
		sprintf(temp, "%d", profession);
		entiy.AddContent(PROFILE_PROFESSION, temp);
	}

	if( children > -1 ) {
		sprintf(temp, "%d", children);
		entiy.AddContent(PROFILE_CHILDREN, temp);
	}

	if( smoke > -1 ) {
		sprintf(temp, "%d", smoke);
		entiy.AddContent(PROFILE_SMOKE, temp);
	}

	if( drink > -1 ) {
		sprintf(temp, "%d", drink);
		entiy.AddContent(PROFILE_DRINK, temp);
	}

	if (income > -1) {
		sprintf(temp, "%d", income);
		entiy.AddContent(PROFILE_INCOME, temp);
	}

	if( resume.length() > 0 ) {
		entiy.AddContent(PROFILE_JJ, resume.c_str());
	}

	string interestString = "";
	for(list<string>::iterator itr = interests.begin(); itr != interests.end(); itr++) {
		interestString += *itr;
		interestString += ",";
	}

	if( interestString.length() > 0 ) {
		interestString = interestString.substr(0, interestString.length() - 1);
		entiy.AddContent(PROFILE_INTERESTS, interestString.c_str());
	}

	string url = UpdateMyProfilePath;
	FileLog("httprequest", "RequestProfileController::UpdateProfile( "
			"url : %s, "
			"weight : %d, "
			"height : %d, "
			"language : %d, "
			"ethnicity : %d, "
			"religion : %d, "
			"education : %d, "
			"profession : %d, "
			"income : %d, "
			"children : %d, "
			"smoke : %d, "
			"drink : %d, "
			"resume : %s, "
			"interestString : %s "
			")",
			url.c_str(),
			weight,
			height,
			language,
			ethnicity,
			religion,
			education,
			profession,
			income,
			children,
			smoke,
			drink,
			resume.c_str(),
			interestString.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 2.3.开始编辑简介触发计时
 * @return				请求唯一标识
 */
long RequestProfileController::StartEditResume() {
	HttpEntiy entiy;

	string url = StartEditMyProfilePath;
	FileLog("httprequest", "RequestProfileController::StartEditResume( "
			"url : %s "
			")",
			url.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 2.4.保存联系电话
 * @param telephone			电话号码
 * @param telephone_cc		国家区号,参考枚举 <RequestEnum.Country>
 * @param landline			固定电话号码
 * @param landline_cc		固定电话号码国家区号,参考枚举 <RequestEnum.Country>
 * @param landline_ac		固话区号
 * @return					请求唯一标识
 */
long RequestProfileController::SaveContact(string telephone, int telephone_cc, string landline,
		int landline_cc, string landline_ac) {
	HttpEntiy entiy;

	if( telephone.length() > 0 ) {
		entiy.AddContent(PROFILE_TELEPHONE, telephone.c_str());
	}

	string strTelephoneCountry = GetCountryCode(telephone_cc);
	if( !strTelephoneCountry.empty() ) {
		entiy.AddContent(PROFILE_TELEPHONE_CC, strTelephoneCountry.c_str());
	}

	if( landline.length() > 0 ) {
		entiy.AddContent(PROFILE_LANDLINE, landline.c_str());
	}

	string strLandlineCountry = GetCountryCode(landline_cc);
	if( !strLandlineCountry.empty() ) {
		entiy.AddContent(PROFILE_LANDLINE_CC, strLandlineCountry.c_str());
	}

	if( landline_ac.length() > 0 ) {
		entiy.AddContent(PROFILE_LANDLINE_AC, landline_ac.c_str());
	}

	string url = SaveContactPath;
	FileLog("httprequest", "RequestProfileController::SaveContact( "
			"url : %s, "
			"telephone : %s, "
			"telephone_cc : %d, "
			"landline : %s, "
			"landline_cc : %d, "
			"landline_ac : %s "
			")",
			url.c_str(),
			telephone.c_str(),
			telephone_cc,
			landline.c_str(),
			landline_cc,
			landline_ac.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 2.5.上传头像
 * @param fileName			文件名
 * @return					请求唯一标识
 */
long RequestProfileController::UploadHeaderPhoto(string fileName) {
	HttpEntiy entiy;

	if( fileName.length() > 0 ) {
		entiy.AddFile(PROFILE_PHOTONAME, fileName);
	}

	string url = UploadPhotoPath;
	FileLog("httprequest", "RequestProfileController::UploadHeaderPhoto( "
			"url : %s, "
			"fileName : %s ",
			url.c_str(),
			fileName.c_str()
			);

	return StartRequest(url, entiy, this);
}
