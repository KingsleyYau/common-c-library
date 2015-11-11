/*
 * KSocket.cpp
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "KSocket.h"

KSocket::KSocket() {
	// TODO Auto-generated constructor stub
	mSocket = -1;
	mbBlocking = false;
}

KSocket::~KSocket() {
	// TODO Auto-generated destructor stub
}

socket_type KSocket::getSocket() {
	return mSocket;
}
void KSocket::setScoket(socket_type socket) {
	mSocket = socket;
}
bool KSocket::setBlocking(bool bBlocking) {
	bool bFlag = false;
	int iFlag = 1;
	if(mSocket != -1) {
		if ((iFlag = fcntl(mSocket, F_GETFL, 0)) != -1) {
			DLog("JNI", "KSocket::setBlocking( fcntl socket(%d) : 0x%x ) \n", mSocket, iFlag);
			if (!bBlocking) {
				// set nonblocking
				if (fcntl(mSocket, F_SETFL, iFlag | O_NONBLOCK) != -1) {
					DLog("common", "KSocket::setBlocking( fcntl set socket(%d) nonblocking ) \n", mSocket);
					mbBlocking = bBlocking;
					bFlag = true;
				}
				else {
					ELog("JNI", "KSocket::setBlocking( fcntl set socket(%d) nonblocking fail ) \n", mSocket);
					bFlag = false;
				}
			}
			else {
				// set blocking
				if (fcntl(mSocket, F_SETFL, iFlag & ~O_NONBLOCK) != -1) {
					DLog("JNI", "KSocket::setBlocking( fcntl set socket(%d) blocking ) \n", mSocket);
					mbBlocking = bBlocking;
					bFlag = true;
				}
				else {
					ELog("JNI", "KSocket::setBlocking( fcntl set socket(%d) blocking ) \n", mSocket);
					bFlag = false;
				}
			}

		}
		else {
			ELog("JNI", "KSocket::setBlocking( fcntl get socket(%d) fail ) \n", mSocket);
			bFlag = false;
		}
	}
	else {
		ELog("JNI", "KSocket::setBlocking( no socket create ) \n");
		bFlag = false;
	}

	if ((iFlag = fcntl(mSocket, F_GETFL, 0)) != -1) {
		DLog("JNI", "KSocket::setBlocking( fcntl get socket(%d) : 0x%x ) \n", mSocket, iFlag);
	}

	return bFlag;
}
bool KSocket::IsBlocking() {
	return mbBlocking;
}
void KSocket::Close() {
	if (mSocket != -1) {
		DLog("JNI", "KSocket::Close( close socket:(%d) ) \n", mSocket);
		shutdown(mSocket, SHUT_RDWR);
		close(mSocket);
		mSocket = -1;
	}
}

unsigned int KSocket::StringToIp(const char* string_ip) {
    unsigned int dwIPAddress = inet_addr(string_ip);

    if (dwIPAddress != INADDR_NONE &&
            dwIPAddress != INADDR_ANY)
    {
        return dwIPAddress;
    }

    return dwIPAddress;
}
string KSocket::IpToString(unsigned int ip_addr) {
    struct in_addr in_ip;
    string stringip = "";
    in_ip.s_addr = ip_addr;
    stringip =  inet_ntoa(in_ip);
    return stringip;
}
unsigned int KSocket::GetTick()
{
	timeval tNow;
	gettimeofday(&tNow, NULL);
	if (tNow.tv_usec != 0){
		return (tNow.tv_sec * 1000 + (unsigned int)(tNow.tv_usec / 1000));
	}else{
		return (tNow.tv_sec * 1000);
	}
}

bool KSocket::IsTimeout(unsigned int uiStart, unsigned int uiTimeout)
{
	DLog("common", "KSocket::IsTimeout( uiStart : %d, uiTimeout:%d ) \n", uiStart, uiTimeout);
	unsigned int uiCurr = GetTick();
	unsigned int uiDiff;

	if (uiTimeout == 0)
		return false;
	if (uiCurr >= uiStart) {
		uiDiff = uiCurr - uiStart;
	}else{
		uiDiff = (0xFFFFFFFF - uiStart) + uiCurr;
	}
	if(uiDiff >= uiTimeout){
		return true;
	}
	return false;
}
