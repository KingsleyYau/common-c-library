/*
 * RequestQuickMatchController.h
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef REQUESTQUICKMATCHCONTROLLER_H_
#define REQUESTQUICKMATCHCONTROLLER_H_

#include "RequestBaseController.h"

#include <list>
using namespace std;

#include "RequestQUICKMATCHDefine.h"

#include "item/QuickMatchLady.h"

#include <json/json/json.h>

typedef void (*OnQueryQuickMatchLadyList)(long requestId, bool success, list<QuickMatchLady> itemList, string errnum, string errmsg);
typedef void (*OnSubmitQuickMatchMarkLadyList)(long requestId, bool success, string errnum, string errmsg);
typedef void (*OnQueryQuickMatchLikeLadyList)(long requestId, bool success, list<QuickMatchLady> itemList, int totalCount, string errnum, string errmsg);
typedef void (*OnRemoveQuickMatchLikeLadyList)(long requestId, bool success, string errnum, string errmsg);

typedef struct RequestQuickMatchControllerCallback {
	OnQueryQuickMatchLadyList onQueryQuickMatchLadyList;
	OnSubmitQuickMatchMarkLadyList onSubmitQuickMatchMarkLadyList;
	OnQueryQuickMatchLikeLadyList onQueryQuickMatchLikeLadyList;
	OnRemoveQuickMatchLikeLadyList onRemoveQuickMatchLikeLadyList;
} RequestQuickMatchControllerCallback;


class RequestQuickMatchController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestQuickMatchController(HttpRequestManager* pHttpRequestManager, RequestQuickMatchControllerCallback callback/*, CallbackManager* pCallbackManager*/);
	virtual ~RequestQuickMatchController();

	/**
	 * 10.1.查询女士图片列表
	 * @param deviceId		设备唯一标识
	 * @return				请求唯一标识
	 */
    long QueryQuickMatchLadyList(string deviceId);

    /**
     * 10.2.提交已标记的女士
     * @param likeList		喜爱的女士列表
     * @param unlikeList	不喜爱女士列表
     * @return				请求唯一标识
     */
    long SubmitQuickMatchMarkLadyList(
    		list<string> likeListId,
    		list<string> unlikeListId
    		);

	/**
	 * 10.3.查询已标记like的女士列表
	 * @return				请求唯一标识
	 */
    long QueryQuickMatchLikeLadyList(
    		int pageIndex,
    		int pageSize
    		);

    /**
     * 10.4.删除已标记like的女士
     * @param likeListId	喜爱的女士列表
     * @return				请求唯一标识
     */
    long RemoveQuickMatchLikeLadyList(list<string> likeListId);

protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	RequestQuickMatchControllerCallback mRequestQuickMatchControllerCallback;
};

#endif /* REQUESTQUICKMATCHCONTROLLER_H_ */
