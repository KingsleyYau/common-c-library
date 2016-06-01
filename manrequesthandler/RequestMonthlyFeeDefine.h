/*
 * RequestMonthlyFeeDefine.h
 *
 *  Created on: 2016-5-10
 *      Author: Hunter
 */

#ifndef REQUESTMONTHLYFEEDEFINE_H_
#define REQUESTMONTHLYFEEDEFINE_H_

#include "RequestDefine.h"

/* ########################	LoveCall 模块  ######################## */

/* 字段 */

/* 13.1.获取月费会员类型 */
#define MONTHLY_FEE_MEMBER_TYPE_PATH    "/member/member_type"

/* 返回参数定义 */
#define MONTHLY_FEE_MEMBER_TYPE          "type"

/* 13.2.获取月费提示层数据 */
#define MONTHLY_FEE_GET_TIPS_PATH    	"/member/month_fee_tip"

/* 返回参数定义 */
#define MONTHLY_FEE_TITLE_PRICE          "title"
#define MONTHLY_FEE_TIPS_LIST         	 "item"


#endif /* REQUESTMONTHLYFEEDEFINE_H_ */
