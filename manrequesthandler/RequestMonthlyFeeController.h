/*
 * RequestMonthlyFeeController.h
 *
 *  Created on: 2016-5-10
 *      Author: Hunter
 */

#ifndef REQUESTMONTHLYFEECONTROLLER_H_
#define REQUESTMONTHLYFEECONTROLLER_H_

#include "RequestBaseController.h"

#include <list>
using namespace std;

#include "RequestMonthlyFeeDefine.h"

#include "item/MonthlyFeeTip.h"

#include <json/json/json.h>

typedef void (*OnQueryMemberType)(long requestId, bool success, string errnum, string errmsg, int memberType);
typedef void (*OnGetMonthlyFeeTips)(long requestId, bool success, string errnum, string errmsg, list<MonthlyFeeTip> tipsList);

typedef struct RequestMonthlyFeeControllerCallback {
	OnQueryMemberType onQueryMemberType;
	OnGetMonthlyFeeTips onGetMonthlyFeeTips;
} RequestMonthlyFeeControllerCallback;


class RequestMonthlyFeeController : public RequestBaseController, public IHttpRequestManagerCallback {
public:
	RequestMonthlyFeeController(HttpRequestManager* pHttpRequestManager, RequestMonthlyFeeControllerCallback callback/*, CallbackManager* pCallbackManager*/);
	virtual ~RequestMonthlyFeeController();

    /**
     * 13.1.获取月费会员类型
     * @return					请求唯一标识
     */
	long QueryMemberType();

    /**
     * 13.2.获取月费提示层数据
     * @return					请求唯一标识
     */
	long GetMonthlyFeeTips();


protected:
	void onSuccess(long requestId, string path, const char* buf, int size);
	void onFail(long requestId, string path);

private:
	RequestMonthlyFeeControllerCallback mRequestMonthlyFeeControllerCallback;
};

#endif /* REQUESTMONTHLYFEECONTROLLER_H_ */
