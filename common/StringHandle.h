/*
 * File         : StringHandle.h
 * Date         : 2012-07-02
 * Author       : Kingsley Yau
 * Copyright    : City Hotspot Co., Ltd.
 * Description  : DrPalm StringHandle include
 */

#ifndef _INC_STRINGHANDLE_
#define _INC_STRINGHANDLE_

#include <ctype.h>
#include "KLog.h"
#include "Arithmetic.h"
#include <list>
using namespace std;
class StringHandle {
public:
	static std::list<std::string> split(std::string str, std::string pattern) {
	    std::string::size_type pos;
	    std::list<std::string> result;
	    str += pattern;
	    int size = str.size();

	    for(int i = 0; i < size; i++) {
	        pos = str.find(pattern, i);
	        if( pos < size ) {
	            std::string s = str.substr(i, pos - i);
	            result.push_back(s);
	            i = pos + pattern.size() - 1;
	        }
	    }
	    return result;
	}

	static inline char* strIstr(const char *haystack, const char *needle) {
	    if (!*needle) {
	    	return (char*)haystack;
	    }
	    for (; *haystack; ++haystack) {
	    	if (toupper(*haystack) == toupper(*needle)) {
	    		const char *h, *n;
	    		for (h = haystack, n = needle; *h && *n; ++h, ++n) {
	    			if (toupper(*h) != toupper(*n)) {
	    				break;
	    			}
	    		}
	    		if (!*n) {
	    			return (char*)haystack;
	    		}
	    	}
	    }
	    return 0;
	}
	static inline string findStringBetween(char* pData, char* pBegin, char* pEnd, char* pTmpBuffer, int iTmpLen) {
		string strRet = "";
		char *pC_Begin = NULL, *pC_End = NULL, *pRep = NULL;
		int iLen = 256;

		if (pTmpBuffer && iTmpLen > 0) {
			pRep = pTmpBuffer;
			iLen = iTmpLen;
		} else {

			pRep = new char[256];

		}
		bzero(pRep, iLen);

		if ((pC_Begin = strIstr(pData, pBegin)) > 0) {

			if ((pC_End = strIstr(pC_Begin, pEnd)) > 0) {

				memcpy(pRep, pC_Begin, pC_End - pC_Begin);
				strRet = pRep;
			}

		}

		if (!pTmpBuffer || iTmpLen <= 0) {
			delete pRep;
		}

		return strRet;
	}
};

#endif


