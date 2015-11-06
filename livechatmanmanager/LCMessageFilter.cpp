/*
 * author: Samson.Fan
 *   date: 2015-10-21
 *   file: LCMessageFilter.cpp
 *   desc: LiveChat文本消息过滤器
 */

#include "LCMessageFilter.h"
#include <common/CommonFunc.h>
#include <regex.h>

static const char* s_pCheckArr[] = { "http://|HTTP://","https://|HTTPS://","ftp://|FTP://",
		"(\\d{3,4}+[-|\\s])*\\d{5,11}","\\d+.\\d+.\\d+.\\d+",
        "(\\d{1,}+[-|\\s]{1,}){3,}+(\\d{1,})*","(\\d{3,}+[\\n|\\r]{1,}){1,}+(\\d{1,})*",
		"([0-9a-zA-Z])*[0-9a-zA-Z]+@([0-9a-zA-Z]+[\\s]{0,}+[.]+[\\s]{0,})+(com|net|cn|org|ru)+[\\s]{0,}",
		"([0-9a-zA-Z]+[-._+&])*([0-9a-zA-Z]+[\\s]{0,}+[.]+[\\s]{0,})+(com|net|cn|org|ru)+[\\s]{1,}",
		" fuck ", " fucking ", " fucked ", " ass ", " asshole ", " cock ", " dick ", " suck ", " sucking ",
		" tit ", " tits ", " nipples ", " horn ", " horny "," pussy ", " wet pussy "," shit "," make love ",
		" making love "," penis "," climax "," lick "," vagina "," sex ", " oral sex ", " anal sex " };

static const char* s_pFilterStr = "******";

// 检测是否带有非法字符消息
bool LCMessageFilter::IsIllegalMessage(const string& message)
{
	bool result = false;

	regex_t ex = {0};
	for (int i = 0; i < _countof(s_pCheckArr); i++)
	{
		if ( 0 == regcomp(&ex, s_pCheckArr[i], REG_EXTENDED|REG_NOSUB) )
		{
			result = (0 == regexec(&ex, message.c_str(), 0, NULL, 0));
			regfree(&ex);

			// 找到可匹配
			if ( result )
			{
				break;
			}
		}
	}

	return result;
}

// 过滤非法字符
string LCMessageFilter::FilterIllegalMessage(const string& message)
{
	string result("");
	regex_t ex = {0};

	// 遍历正则表达式
	for (int i = 0; i < _countof(s_pCheckArr); i++)
	{
		// 新建正则表达式对象
		if ( 0 == regcomp(&ex, s_pCheckArr[i], REG_EXTENDED) )
		{
			// 查找匹配字符串，并替换
			size_t begin = 0;
			while (true)
			{
				regmatch_t pm = {0};
				if ( 0 == regexec(&ex, message.c_str() + begin, 1, &pm, 0) )
				{
					// 找到匹配字符串
					if ( pm.rm_so > 0 ) {
						// 添加匹配位置之前的字符
						result += message.substr(begin, pm.rm_so);
					}
					// 起始位置偏移，过滤匹配字符
					begin += pm.rm_eo;
					// 添加替换字符
					result += s_pFilterStr;
				}
				else
				{
					// 添加没有匹配成功的字符
					result += message.substr(begin);
					break;
				}
			}

			// 释放正则表达式对象
			regfree(&ex);
		}
	}

	return result;
}
