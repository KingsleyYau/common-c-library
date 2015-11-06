/*
 * RequestLoveCallController.h
 *
 *  Created on: 2015-2-27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef REQUESTLOVECALLCONTROLLER_H_
#define REQUESTLOVECALLCONTROLLER_H_

#include "RequestBaseController.h"

#include <list>
using namespace std;

#include "RequestLoveCallDefine.h"

#include "item/LoveCall.h"

#include <json/json/json.h>

typedef void (*OnQueryLoveCallList)(long requestId, bool success, list<LoveCall> itemList, int totalCount, string errnum, string errmsg);
typedef void (*OnConfirmLoveCall)(long requestId, bool success, string errnum, string errmsg);
typedef void (*OnQueryLoveCallRequestCount)(long requestId, bool success, string errnum, string errmsg, int num);

typedef struct RequestLoveCallControllerCallback {
	OnQueryLoveCallList onQueryLoveCallList;
	OnConfirmLoveCall onConfirmLoveCall;
	OnQueryLoveCallRequestCount onQueryLoveCallRequestCount;
} RequestLoveCallControllerCallback;


class RequestLoveCallController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestLoveCallController(HttpRequestManager* pHttpRequestManager, RequestLoveCallControllerCallback callback/*, CallbackManager* pCallbackManager*/);
	virtual ~RequestLoveCallController();

    /**
     * 11.1.获取Love Call列表接口
     * @param pageIndex			当前页数
     * @param pageSize			每页行数
     * @param searchType		查询类型（0：Request，1：Scheduled）
     * @return					请求唯一标识
     */
	long QueryLoveCallList(int pageIndex, int pageSize, int searchType);

    /**
     * 11.2.确定Love Call接口
     * @param orderId			订单ID
     * @param confirmType		确定类型（1：接受，0：拒绝）
     * @return					请求唯一标识
     */
	long ConfirmLoveCall(string orderId, int confirmType);

	// 11.3 获取LoveCall未处理数
	long QueryLoveCallRequestCount(int searchType);

protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	RequestLoveCallControllerCallback mRequestLoveCallControllerCallback;
};

#endif /* REQUESTLOVECALLCONTROLLER_H_ */
