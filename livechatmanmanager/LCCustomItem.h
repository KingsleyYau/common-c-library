/*
 * author: Samson.Fan
 *   date: 2016-05-28
 *   file: LCCustomItem.h
 *   desc: LiveChat自定义消息item
 */

#pragma once

#include <string>
using namespace std;

class LCCustomItem
{
public:
	LCCustomItem();
	virtual ~LCCustomItem();

public:
	bool Init(long param);

public:
	long m_param;		// 消息参数
};
