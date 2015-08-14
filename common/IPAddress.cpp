/*
 * IPAddress.cpp
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#include "IPAddress.h"

#include <resolv.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>

#include "KLog.h"

#define BUFFERSIZE    4000
#define    min(a,b)    ((a) < (b) ? (a) : (b))
#define    max(a,b)    ((a) > (b) ? (a) : (b))

IPAddress::IPAddress() {
	// TODO Auto-generated constructor stub
}

IPAddress::~IPAddress() {
	// TODO Auto-generated destructor stub
}
list<IpAddressNetworkInfo> IPAddress::GetNetworkInfoList() {
	list<IpAddressNetworkInfo> valueList;
	int fd = 0, intrface = 0, iCount = 1;
	struct ifconf ifc;
	struct ifreq buf[16] = {'\0'};
	struct sockaddr_in* sin;
	char cBuffer[256] = {'\0'};

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (char*)buf;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0){
				ifreq *ifr_ip = &buf[intrface];
				IpAddressNetworkInfo info;

				info.name = ifr_ip->ifr_name;

				// MAC
				if (!(ioctl(fd, SIOCGIFHWADDR, (char *) ifr_ip))) {
					sprintf(cBuffer, "%02X:%02X:%02X:%02X:%02X:%02X", ifr_ip->ifr_hwaddr.sa_data[0], ifr_ip->ifr_hwaddr.sa_data[1],
							ifr_ip->ifr_hwaddr.sa_data[2], ifr_ip->ifr_hwaddr.sa_data[3], ifr_ip->ifr_hwaddr.sa_data[4],
							ifr_ip->ifr_hwaddr.sa_data[5]);
					info.mac = cBuffer;

					for(int i = 0; i < 6; i++) {
						info.macByte[i] = ifr_ip->ifr_hwaddr.sa_data[i];
					}
				}

				// IP
				if (!(ioctl(fd, SIOCGIFADDR, (char *) ifr_ip))) {
					sin = (struct sockaddr_in *)&ifr_ip->ifr_addr;
					info.ip = inet_ntoa(sin->sin_addr);
				}

				// 广播地址
				if (!(ioctl(fd, SIOCGIFBRDADDR, (char *) ifr_ip))) {
					sin = (struct sockaddr_in *)&ifr_ip->ifr_broadaddr;
					info.broad = inet_ntoa(sin->sin_addr);
				}

				// 子网掩码
				if(!(ioctl(fd, SIOCGIFNETMASK, (char *) ifr_ip))) {
					sin = (struct sockaddr_in *)&ifr_ip->ifr_netmask;
					info.netmask = inet_ntoa(sin->sin_addr);
				}

				// 点到点地址
				if(!(ioctl(fd, SIOCGIFDSTADDR, (char *) ifr_ip))) {
					sin = (struct sockaddr_in *)&ifr_ip->ifr_dstaddr;
					info.dst = inet_ntoa(sin->sin_addr);
				}

				if (!(ioctl (fd, SIOCGIFFLAGS, (char *) ifr_ip))) {
					// 判断网卡状态
					if (ifr_ip->ifr_flags & IFF_UP) {
						info.bUp = true;
					}

//					// 判断网卡类型, 是否PPPoE
//					if(ifr_ip->ifr_flags & IF_PROTO_PPP) {
//						info.bPPP = true;
//					}
				}

				DLog("jni.IPAddress::GetNetworkInfoList", "#################################");
				DLog("jni.IPAddress::GetNetworkInfoList", "info.name:%s", info.name.c_str());
				DLog("jni.IPAddress::GetNetworkInfoList", "info.ip:%s", info.ip.c_str());
				DLog("jni.IPAddress::GetNetworkInfoList", "info.netmask:%s", info.netmask.c_str());
				DLog("jni.IPAddress::GetNetworkInfoList", "info.broad:%s", info.broad.c_str());
				DLog("jni.IPAddress::GetNetworkInfoList", "info.dst:%s", info.dst.c_str());
				DLog("jni.IPAddress::GetNetworkInfoList", "info.mac:%s", info.mac.c_str());
				DLog("jni.IPAddress::GetNetworkInfoList", "info.bUp:%d", info.bUp);
				DLog("jni.IPAddress::GetNetworkInfoList", "info.bPPP:%d", info.bPPP);
				DLog("jni.IPAddress::GetNetworkInfoList", "#################################");

				valueList.push_back(info);
			}
		}
	}
	close(fd);
	return valueList;
}
list<string> IPAddress::GetDeviceList() {
	list<string> valueList;
	int fd = 0, intrface = 0, iCount = 1;
	struct ifconf ifc;
	struct ifreq buf[16] = {'\0'};
	struct sockaddr_in* sin;
	char cBuffer[256] = {'\0'};

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (char*)buf;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0){
				ifreq ifr_ip = buf[intrface];
				string name = ifr_ip.ifr_name;
				valueList.push_back(name);
//				showLog("jni.IPAddress::GetDeviceList", "ifr_name:%s", name.c_str());
			}
		}
	}
	close(fd);
	return valueList;
}
list<string> IPAddress::GetMacAddressList() {
	list<string> valueList;
	int fd = 0, intrface = 0, iCount = 1;
	struct ifconf ifc;
	struct ifreq buf[16];
	char cBuffer[256] = {'\0'};

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (caddr_t)buf;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0){
				ifreq ifr_ip = buf[intrface];
				if (!(ioctl(fd, SIOCGIFHWADDR, (char *) &buf[intrface]))) {
					sprintf(cBuffer, "%02X:%02X:%02X:%02X:%02X:%02X", buf[intrface].ifr_hwaddr.sa_data[0], buf[intrface].ifr_hwaddr.sa_data[1],
							buf[intrface].ifr_hwaddr.sa_data[2], buf[intrface].ifr_hwaddr.sa_data[3], buf[intrface].ifr_hwaddr.sa_data[4],
							buf[intrface].ifr_hwaddr.sa_data[5]);
					valueList.push_back(cBuffer);
				}
			}
		}
	}
	close(fd);
	return valueList;
}
list<string> IPAddress::GetIPAddress() {
	list<string> valueList;
	int fd = 0, intrface = 0, iCount = 1;
	struct ifconf ifc;
	struct ifreq buf[16];
	struct sockaddr_in* sin;
	char cBuffer[256] = {'\0'};

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (char*)buf;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0){
				ifreq *ifr_ip = &buf[intrface];
				if (!(ioctl(fd, SIOCGIFADDR, (char *) ifr_ip))) {
					struct sockaddr_in* sin = (struct sockaddr_in *)&ifr_ip->ifr_addr;
					strcpy(cBuffer, inet_ntoa(sin->sin_addr));
					valueList.push_back(cBuffer);
				}

//				showLog("jni.IPAddress::GetIPAddress", "ip:%s", cBuffer);
			}
		}
	}
	close(fd);
	return valueList;
}
list<string> IPAddress::GetBroadAddress() {
	list<string> valueList;
	int fd = 0, intrface = 0, iCount = 1;
	struct ifconf ifc;
	struct ifreq buf[16];
	struct sockaddr_in* sin;
	char cBuffer[256] = {'\0'};

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (char*)buf;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc)) {
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0){
				ifreq *ifr_ip = &buf[intrface];
				if (!(ioctl(fd, SIOCGIFBRDADDR, (char *) ifr_ip))) {
					struct sockaddr_in* sin = (struct sockaddr_in *)&ifr_ip->ifr_broadaddr;
					strcpy(cBuffer, inet_ntoa(sin->sin_addr));
					valueList.push_back(cBuffer);
				}
//				showLog("jni.IPAddress::GetBroadAddress", "ip:%s", cBuffer);
			}
		}
	}
	close(fd);
	return valueList;
}

