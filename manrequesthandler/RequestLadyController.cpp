/*
 * RequestLadyController.cpp
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "RequestLadyController.h"

RequestLadyController::RequestLadyController(HttpRequestManager *phttprequestManager, RequestLadyControllerCallback callback/* CallbackManager* pCallbackManager*/) {
	// TODO Auto-generated constructor stub
	SetHttpRequestManager(phttprequestManager);
	mRequestLadyControllerCallback = callback;
}

RequestLadyController::~RequestLadyController() {
	// TODO Auto-generated destructor stub
}

/* IhttprequestManagerCallback */
void RequestLadyController::onSuccess(long requestId, string url, const char* buf, int size) {
	FileLog("httprequest", "RequestLadyController::onSuccess( url : %s, buf( size : %d ) )", url.c_str(), size);
	if (size < MAX_LOG_BUFFER) {
		FileLog("httprequest", "RequestLadyController::onSuccess(), buf: %s", buf);
	}

	/* parse base result */
	string errnum = "";
	string errmsg = "";
	Json::Value data;

	bool bFlag = HandleResult(buf, size, errnum, errmsg, &data);

	/* resopned parse ok, callback success */
	if( url.compare(QUERY_LADY_MATCH_PATH) == 0 ) {
		/* 4.1.获取匹配女士条件 */
		LadyMatch item;
		item.Parse(data);
		if( mRequestLadyControllerCallback.onQueryLadyMatch != NULL ) {
			mRequestLadyControllerCallback.onQueryLadyMatch(requestId, bFlag, item, errnum, errmsg);
		}
	} else if( url.compare(SAVE_LADY_MATCH_PATH) == 0 ) {
		/* 4.2.保存匹配女士条件 */
		if( mRequestLadyControllerCallback.onSaveLadyMatch != NULL ) {
			mRequestLadyControllerCallback.onSaveLadyMatch(requestId, bFlag, errnum, errmsg);
		}
	} else if( url.compare(QUERY_LADY_LIST_PATH) == 0 ) {
		/* 4.3.条件查询女士列表 */
		list<Lady> ladyList;
		int totalCount = 0;
		if( data[COMMON_DATA_COUNT].isInt() ) {
			totalCount = data[COMMON_DATA_COUNT].asInt();
		}
		if( data[COMMON_DATA_LIST].isArray() ) {
			for(int i = 0; i < data[COMMON_DATA_LIST].size(); i++ ) {
				Lady lady;
				lady.Parse(data[COMMON_DATA_LIST].get(i, Json::Value::null));
				ladyList.push_back(lady);
			}
		} else {
			// parsing fail
			bFlag = false;
			errnum = LOCAL_ERROR_CODE_PARSEFAIL;
			errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;
		}

		if( mRequestLadyControllerCallback.onQueryLadyList != NULL ) {
			mRequestLadyControllerCallback.onQueryLadyList(requestId, bFlag, ladyList,
					totalCount, errnum, errmsg);
		}
	} else if( url.compare(QUERY_LADY_DETAIL_PATH) == 0 ) {
		/* 4.4.查询女士详细信息 */
		LadyDetail item;
		item.Parse(data);
		if( mRequestLadyControllerCallback.onQueryLadyDetail != NULL ) {
			mRequestLadyControllerCallback.onQueryLadyDetail(requestId, bFlag, item, errnum, errmsg);
		}
	} else if( url.compare(ADD_FAVORITES_LADY_PATH) == 0 ) {
		/* 4.5.收藏女士 */
		if( mRequestLadyControllerCallback.onAddFavouritesLady != NULL ) {
			mRequestLadyControllerCallback.onAddFavouritesLady(requestId, bFlag, errnum, errmsg);
		}
	} else if( url.compare(REMOVE_FAVORITES_LADY_PATH) == 0 ) {
		/* 4.6.删除收藏女士 */
		if( mRequestLadyControllerCallback.onRemoveFavouritesLady != NULL ) {
			mRequestLadyControllerCallback.onRemoveFavouritesLady(requestId, bFlag, errnum, errmsg);
		}
	} else if( url.compare(QUERY_DIRECT_CALL_LADY_PATH) == 0 ) {
		/* 4.7.获取女士Direct Call TokenID */
		LadyCall item;
		item.Parse(data);
		if( mRequestLadyControllerCallback.onQueryLadyCall != NULL ) {
			mRequestLadyControllerCallback.onQueryLadyCall(requestId, bFlag, item, errnum, errmsg);
		}
	} else if ( url.compare(QUERY_RECENTCONTACT_PATH) == 0 ) {
		/* 4.8.获取最近联系人列表（ver3.0起） */
		list<LadyRecentContact> list;
		if( data[COMMON_DATA_LIST].isArray() ) {
			for(int i = 0; i < data[COMMON_DATA_LIST].size(); i++ ) {
				LadyRecentContact item;
				if (item.Parse(data[COMMON_DATA_LIST].get(i, Json::Value::null))) {
					list.push_back(item);
				}
			}
		} else {
			// parsing fail
			bFlag = false;
			errnum = LOCAL_ERROR_CODE_PARSEFAIL;
			errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;
		}

		if (mRequestLadyControllerCallback.onRecentContact != NULL) {
			mRequestLadyControllerCallback.onRecentContact(requestId, bFlag, errnum, errmsg, list);
		}
	}else if( url.compare(QUERY_REMOVECONTACT_PATH) == 0 ){
		/* 4.9 删除联系人返回 3.0.3开始使用 */
		if( mRequestLadyControllerCallback.onRemoveContactList != NULL ) {
			mRequestLadyControllerCallback.onRemoveContactList(requestId, bFlag, errnum, errmsg);
		}
	} else if ( url.compare(QUERY_SIGNLIST_PATH) == 0 ) {
		/* 4.9.查询女士标签列表（ver3.0起） */
		list<LadySignListItem> list;
		if( data[COMMON_DATA_LIST].isArray() ) {
			for(int i = 0; i < data[COMMON_DATA_LIST].size(); i++ ) {
				LadySignListItem item;
				if (item.Parse(data[COMMON_DATA_LIST].get(i, Json::Value::null))) {
					list.push_back(item);
				}
			}
		} else {
			// parsing fail
			bFlag = false;
			errnum = LOCAL_ERROR_CODE_PARSEFAIL;
			errmsg = LOCAL_ERROR_CODE_PARSEFAIL_DESC;
		}

		if (mRequestLadyControllerCallback.onSignList != NULL) {
			mRequestLadyControllerCallback.onSignList(requestId, bFlag, errnum, errmsg, list);
		}
	} else if ( url.compare(QUERY_UPLOADSIGN_PATH) == 0 ) {
		/* 4.10.提交女士标签（ver3.0起） */
		if( mRequestLadyControllerCallback.onUploadSign != NULL ) {
			mRequestLadyControllerCallback.onUploadSign(requestId, bFlag, errnum, errmsg);
		}
    } else if ( url.compare(REPORT_LADY_PATH) == 0 ) {
        /* 举报女士 */
        if ( mRequestLadyControllerCallback.onReportLady != NULL ) {
            mRequestLadyControllerCallback.onReportLady(requestId, bFlag, errnum, errmsg);
        }
    }

	FileLog("httprequest", "RequestLadyController::onSuccess() url: %s, end", url.c_str());
}
void RequestLadyController::onFail(long requestId, string url) {
	FileLog("httprequest", "RequestLadyController::onFail( url : %s )", url.c_str());
	/* request fail, callback fail */
	if( url.compare(QUERY_LADY_MATCH_PATH) == 0 ) {
		/* 4.1.获取匹配女士条件 */
		LadyMatch item;
		if( mRequestLadyControllerCallback.onQueryLadyMatch != NULL ) {
			mRequestLadyControllerCallback.onQueryLadyMatch(requestId, false, item, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(SAVE_LADY_MATCH_PATH) == 0 ) {
		/* 4.2.保存匹配女士条件 */
		if( mRequestLadyControllerCallback.onSaveLadyMatch != NULL ) {
			mRequestLadyControllerCallback.onSaveLadyMatch(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(QUERY_LADY_LIST_PATH) == 0 ) {
		/* 4.3.条件查询女士列表 */
		list<Lady> ladyList;
		if( mRequestLadyControllerCallback.onQueryLadyList != NULL ) {
			mRequestLadyControllerCallback.onQueryLadyList(requestId, false, ladyList, 0, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(QUERY_LADY_DETAIL_PATH) == 0 ) {
		/* 4.4.查询女士详细信息 */
		LadyDetail item;
		if( mRequestLadyControllerCallback.onQueryLadyDetail != NULL ) {;
			mRequestLadyControllerCallback.onQueryLadyDetail(requestId, false, item, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(ADD_FAVORITES_LADY_PATH) == 0 ) {
		/* 4.5.收藏女士 */
		if( mRequestLadyControllerCallback.onAddFavouritesLady != NULL ) {
			mRequestLadyControllerCallback.onAddFavouritesLady(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(REMOVE_FAVORITES_LADY_PATH) == 0 ) {
		/* 4.6.删除收藏女士 */
		if( mRequestLadyControllerCallback.onRemoveFavouritesLady != NULL ) {
			mRequestLadyControllerCallback.onRemoveFavouritesLady(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if( url.compare(QUERY_DIRECT_CALL_LADY_PATH) == 0 ) {
		/* 4.7.获取女士Direct Call TokenID */
		LadyCall item;
		if( mRequestLadyControllerCallback.onQueryLadyCall != NULL ) {
			mRequestLadyControllerCallback.onQueryLadyCall(requestId, false, item, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
	} else if ( url.compare(QUERY_RECENTCONTACT_PATH) == 0 ) {
		/* 4.8.获取最近联系人列表（ver3.0起） */
		if( mRequestLadyControllerCallback.onRecentContact != NULL ) {
			list<LadyRecentContact> list;
			mRequestLadyControllerCallback.onRecentContact(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC, list);
		}
	} else if ( url.compare(QUERY_SIGNLIST_PATH) == 0 ) {
		/* 4.10.查询女士标签列表（ver3.0起） */
		if( mRequestLadyControllerCallback.onSignList != NULL ) {
			list<LadySignListItem> list;
			mRequestLadyControllerCallback.onSignList(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC, list);
		}
	} else if ( url.compare(QUERY_UPLOADSIGN_PATH) == 0 ) {
		/* 4.10.提交女士标签（ver3.0起） */
		if( mRequestLadyControllerCallback.onUploadSign != NULL ) {
			mRequestLadyControllerCallback.onUploadSign(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
		}
    } else if ( url.compare(REPORT_LADY_PATH) == 0 ) {
        /* 举报女士 */
        if ( mRequestLadyControllerCallback.onReportLady != NULL ) {
            mRequestLadyControllerCallback.onReportLady(requestId, false, LOCAL_ERROR_CODE_TIMEOUT, LOCAL_ERROR_CODE_TIMEOUT_DESC);
        }
    }

	FileLog("httprequest", "RequestLadyController::onFail() url: %s, end", url.c_str());
}

/**
 * 4.1.获取匹配女士条件
 * @param callback
 * @return			请求唯一标识
 */
long RequestLadyController::QueryLadyMatch() {
	HttpEntiy entiy;

	string url = QUERY_LADY_MATCH_PATH;
	FileLog("httprequest", "RequestLadyController::QueryLadyMatch( url : %s )",
			url.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 4.2.保存匹配女士条件
 * @param ageRangeFrom		起始年龄
 * @param ageRangeTo		结束年龄
 * @param marry				婚姻状况
 * @param children			子女状况
 * @param education			教育程度
 * @param callback
 * @return					请求唯一标识
 */
long RequestLadyController::SaveLadyMatch(int ageRangeFrom, int ageRangeTo, int marry,
		int children, int education) {
	char temp[16];

	HttpEntiy entiy;

	sprintf(temp, "%d", ageRangeFrom);
	entiy.AddContent(LADY_AGE_RANGE_FROM, temp);

	sprintf(temp, "%d", ageRangeTo);
	entiy.AddContent(LADY_AGE_RANGE_TO, temp);

	sprintf(temp, "%d", marry);
	entiy.AddContent(LADY_M_MARRY, temp);

	sprintf(temp, "%d", children);
	entiy.AddContent(LADY_M_CHILDREN, temp);

	sprintf(temp, "%d", education);
	entiy.AddContent(LADY_M_EDUCATION, temp);

	string url = SAVE_LADY_MATCH_PATH;
	FileLog("httprequest", "RequestLadyController::SaveLadyMatch( "
			"url : %s, "
			"ageRangeFrom : %d, "
			"ageRangeTo : %d, "
			"marry : %d, "
			"children : %d, "
			"education : %d "
			")",
			url.c_str(),
			ageRangeFrom,
			ageRangeTo,
			marry,
			children,
			education);

	return StartRequest(url, entiy, this);
}

/**
 * 4.3.条件查询女士列表
 * @param pageIndex			当前页数
 * @param pageSize			每页行数
 * @param searchType		查询类型,参考枚举
 * @param womanId			女士ID
 * @param isOnline			是否在线
 * @param ageRangeFrom		起始年龄
 * @param ageRangeTo		结束年龄
 * @param country			国家
 * @return					请求唯一标识
 */
long RequestLadyController::QueryLadyList(int pageIndex, int pageSize, int searchType, const string& womanId,
		int isOnline, int ageRangeFrom, int ageRangeTo, const string& country, int orderBy, const string& deviceId,
        LadyGenderType genderType) {
	char temp[16];

	HttpEntiy entiy;

	sprintf(temp, "%d", pageIndex);
	entiy.AddContent(COMMON_PAGE_INDEX, temp);

	sprintf(temp, "%d", pageSize);
	entiy.AddContent(COMMON_PAGE_SIZE, temp);

	sprintf(temp, "%d", searchType);
	entiy.AddContent(LADY_QUERY_TYPE, temp);

	if( womanId.length() > 0 ) {
		entiy.AddContent(LADY_WOMAN_ID, womanId.c_str());
	}

	if ( isOnline >= 0 ) {
		sprintf(temp, "%d", isOnline);
		entiy.AddContent(LADY_ISONLINE, temp);
	}

	if( ageRangeFrom >= 0 ) {
		sprintf(temp, "%d", ageRangeFrom);
		entiy.AddContent(LADY_AGE_RANGE_FROM, temp);
	}

	if( ageRangeTo >= 0 ) {
		sprintf(temp, "%d", ageRangeTo);
		entiy.AddContent(LADY_AGE_RANGE_TO, temp);
	}

	if( country.length() > 0 ) {
		entiy.AddContent(LADY_COUNTRY, country.c_str());
	}

	if( orderBy >= 0 ) {
		sprintf(temp, "%d", orderBy);
		entiy.AddContent(LADY_ORDERBY, temp);
	}

	if ( deviceId.length() > 0 ) {
		entiy.AddContent(LADY_DEVICEID, deviceId.c_str());
	}
    
    // 用于iOS假服务器
    if (genderType != LADY_GENDER_DEFAULT) {
        entiy.AddContent(LADY_GENDER, LadyGenderProtocol[genderType]);
    }

	string url = QUERY_LADY_LIST_PATH;
	FileLog("httprequest", "RequestLadyController::QueryLadyList( "
			"url : %s, "
			"pageIndex : %d, "
			"pageSize : %d, "
			"searchType : %d, "
			"womanId : %s, "
			"isOnline : %d, "
			"ageRangeFrom : %d, "
			"ageRangeTo : %d, "
			"country : %s "
			"orderBy : %d "
			"deviceId : %s "
            "genderType : %d"
			")",
			url.c_str(),
			pageIndex,
			pageSize,
			searchType,
			womanId.c_str(),
			isOnline,
			ageRangeFrom,
			ageRangeTo,
			country.c_str(),
			orderBy,
			deviceId.c_str(),
            genderType);

	return StartRequest(url, entiy, this);
}

/**
 * 4.4.查询女士详细信息
 * @param womanId			女士ID
 * @return					请求唯一标识
 */
long RequestLadyController::QueryLadyDetail(string womanId) {
	HttpEntiy entiy;

	if( womanId.length() > 0 ) {
		entiy.AddContent(LADY_WOMAN_ID, womanId);
	}

	string url = QUERY_LADY_DETAIL_PATH;
	FileLog("httprequest", "RequestLadyController::QueryLadyDetail( "
			"url : %s, "
			"womanId : %s "
			")",
			url.c_str(),
			womanId.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 4.5.收藏女士
 * @param womanId			女士ID
 * @return					请求唯一标识
 */
long RequestLadyController::AddFavouritesLady(string womanId) {
	HttpEntiy entiy;

	if( womanId.length() > 0 ) {
		entiy.AddContent(LADY_WOMAN_ID, womanId);
	}

	string url = ADD_FAVORITES_LADY_PATH;
	FileLog("httprequest", "RequestLadyController::AddFavouritesLady( "
			"url : %s, "
			"womanId : %s, "
			")",
			url.c_str(),
			womanId.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 4.6.删除收藏女士
 * @param womanId			女士ID
 * @return					请求唯一标识
 */
long RequestLadyController::RemoveFavouritesLady(string womanId) {
	HttpEntiy entiy;

	if( womanId.length() > 0 ) {
		entiy.AddContent(LADY_WOMAN_ID, womanId);
	}

	string url = REMOVE_FAVORITES_LADY_PATH;
	FileLog("httprequest", "RequestLadyController::RemoveFavouritesLady( "
			"url : %s, "
			"womanId : %s, "
			")",
			url.c_str(),
			womanId.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 4.7.获取女士Direct Call TokenID
 * @param womanId			女士ID
 * @return					请求唯一标识
 */
long RequestLadyController::QueryLadyCall(string womanId) {
	HttpEntiy entiy;

	if( womanId.length() > 0 ) {
		entiy.AddContent(LADY_WOMAN_ID, womanId);
	}

	string url = QUERY_DIRECT_CALL_LADY_PATH;
	FileLog("httprequest", "RequestLadyController::QueryLadyCall( "
			"url : %s, "
			"womanId : %s, "
			")",
			url.c_str(),
			womanId.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 获取最近联系人列表（ver3.0起）
 * @return					请求唯一标识
 */
long RequestLadyController::RecentContactList()
{
	HttpEntiy entiy;

	string url = QUERY_RECENTCONTACT_PATH;
	FileLog("httprequest", "RequestLadyController::RecentContactList( "
			"url : %s, "
			")",
			url.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 删除最近联系人（ver3.0.3起）
 * @param womanListId	被删除的女士Id列表
 * @return				请求唯一标识
 */
long RequestLadyController::RemoveContactList(list<string> womanListId) {

	HttpEntiy entiy;

	string womanId = "";
	for(list<string>::iterator itr = womanListId.begin(); itr != womanListId.end(); itr++) {
		womanId += *itr;
		womanId += ",";
	}
	if( womanId.length() > 0 ) {
		womanId = womanId.substr(0, womanId.length() - 1);
		entiy.AddContent(REMOVE_CONTACT_WOMANID, womanId);
	}

	string url = QUERY_REMOVECONTACT_PATH;
	FileLog("httprequest", "RequestLadyController::RemoveContactList( "
			"url : %s, "
			"womanId : %s "
			")",
			url.c_str(),
			womanId.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 查询女士标签列表（ver3.0起）
 * @param womanId			女士ID
 * @return					请求唯一标识
 */
long RequestLadyController::QuerySignList(const string& womanId)
{
	HttpEntiy entiy;

	if( !womanId.empty() ) {
		entiy.AddContent(LADY_WOMAN_ID, womanId);
	}

	string url = QUERY_SIGNLIST_PATH;
	FileLog("httprequest", "RequestLadyController::QuerySignList( "
			"url : %s, "
			"womanId : %s, "
			")",
			url.c_str(),
			womanId.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 * 提交女士标签（ver3.0起）
 * @param womanId			女士ID
 * @param signIds			选中的标签列表
 * @return					请求唯一标识
 */
long RequestLadyController::UploadSigned(const string& womanId, const list<string> signIds)
{
	HttpEntiy entiy;

	if( !womanId.empty() ) {
		entiy.AddContent(LADY_WOMAN_ID, womanId);
	}

	string strSignIds("");
	list<string>::const_iterator iter;
	for (iter = signIds.begin(); iter != signIds.end(); iter++) {
		if (!strSignIds.empty()) {
			strSignIds += ",";
		}
		strSignIds += (*iter);
	}
	entiy.AddContent(LADY_SIGNID, strSignIds);

	string url = QUERY_UPLOADSIGN_PATH;
	FileLog("httprequest", "RequestLadyController::UploadSigned( "
			"url : %s, "
			"womanId : %s, "
			"signId : %s, "
			")",
			url.c_str(),
			womanId.c_str(),
			strSignIds.c_str()
			);

	return StartRequest(url, entiy, this);
}

/**
 *  举报女士
 *  @param womanId          女士ID
 *  @return                 请求唯一标识
 */
long RequestLadyController::ReportLady(const string& womanId)
{
    HttpEntiy entiy;
    
    if( !womanId.empty() ) {
        entiy.AddContent(LADY_WOMAN_ID, womanId);
    }
    
    string url = REPORT_LADY_PATH;
    FileLog("httprequest", "RequestLadyController::ReportLady( "
            "url : %s, "
            "womanId : %s"
            ")",
            url.c_str(),
            womanId.c_str()
            );
    
    return StartRequest(url, entiy, this);
}
