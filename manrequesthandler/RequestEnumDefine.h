/*
 * RequestEnumDefine.h
 *
 *  Created on: 2015-3-6
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef REQUESTENUMDEFINE_H_
#define REQUESTENUMDEFINE_H_

#include <string>
using namespace std;

// 获取指定位置的国家代码
string GetCountryCode(int code);
// 获取指定国家代码的位置
int GetCountryCode(const string& code);
// 获取其它国家代码的位置
int GetOtherCountryCode();

#endif /* REQUESTENUMDEFINE_H_ */
