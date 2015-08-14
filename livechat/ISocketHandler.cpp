/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: ISocketHandler.cpp
 *   desc: 跨平台socket处理接口类
 */

#include "ISocketHandler.h"


#ifdef WIN32
class WinTcpSocketHandler : public ISocketHandler
{
public:
	WinTcpSocketHandler() {
		m_socket = INVALID_SOCKET;
		m_block = false;
	}
	virtual ~WinTcpSocketHandler() {
		Close();
	}

public:
	// 创建socket
	virtual bool Create() {
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		return INVALID_SOCKET != m_socket;
	}
	
	// 停止socket
	virtual void Shutdown()
	{
		if (INVALID_SOCKET != m_socket)
		{
			shutdown(m_socket, SHUT_RDWR);
		}
	}

	// 关闭socket
	virtual void Close() {
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		m_block = false;
	}
	
	// 绑定ip
	virtual bool Bind(const string& ip, unsigned int port) {
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = inet_addr(ip.c_str());
		service.sin_port = htons(port);

		return SOCKET_ERROR != bind(m_socket, (SOCKADDR*)&service, sizeof(service));
	}
	
	// 连接（msTimeout：超时时间(毫秒)，不大于0表示使用默认超时）
	virtual SOCKET_RESULT_CODE Connect(const string& ip, unsigned int port, int msTimeout) {
		SOCKET_RESULT_CODE result = SOCKET_RESULT_FAIL;

		// 定义socketaddr
		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(ip.c_str());
		server.sin_port = htons(port);

		// 连接
		if (msTimeout > 0) {
			SetBlock(false);
			if (connect(m_socket, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAEWOULDBLOCK) {
					timeval timeout;  
					timeout.tv_sec = msTimeout / 1000;
					timeout.tv_usec = msTimeout % 1000;
					fd_set writeset, exceptset;
					FD_ZERO(&writeset);
					FD_SET(m_socket, &writeset);
					FD_ZERO(&exceptset);
					FD_SET(m_socket, &exceptset);
		  
					int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
					if (ret == 0) {
						result = SOCKET_RESULT_TIMEOUT;
					}
					else if (ret > 0 && 0 != FD_ISSET(m_socket, &exceptset)) {
						result  = SOCKET_RESULT_SUCCESS;
					}
				}
				else if (WSAGetLastError() == 0) {
					result = SOCKET_RESULT_SUCCESS;
				}
			}
			else {
				result = SOCKET_RESULT_SUCCESS;
			}
			SetBlock(true);
		}
		else {
			SetBlock(true);
			if (connect(m_socket, (SOCKADDR*)&server, sizeof(server)) == 0) {
				result = SOCKET_RESULT_SUCCESS;
			}
		}
		return result;
	}
	
	// 发送
	virtual bool Send(void* data, unsigned int dataLen) {
		bool result = false;

		int total = 0;
		while (true) {
			int iSent = send(m_socket, (const char*)data, dataLen, 0);
			if (iSent > 0) {
				total += iSent;
			}
			else {
				break;
			}

			if (total == dataLen) {
				result = true;
				break;
			}
		}
		return result;
	}
	
	// 接收
	virtual bool Recv(void* data, unsigned int dataSize, unsigned int& dataLen) {
		bool result = false;

		dataLen = 0;
		int length = recv(m_socket, (char*)data, dataSize, 0);
		if (length > 0) {
			dataLen = length;
			result = true;
		}

		return result;
	}

private:
	// blocking设置
	bool SetBlock(bool block) {
		bool result = true;
		if (m_block != block) {
			unsigned long mode = block ? 0 : 1;
			result = ioctlsocket(m_socket, FIONBIO, &mode) == 0;

			if (result) {
				m_block = block;
			}
		}
		return result;
	}

private:
	SOCKET	m_socket;
	bool	m_block;
};

#else
class LinuxTcpSocketHandler : public ISocketHandler
{
public:
	LinuxTcpSocketHandler() {
		m_socket = INVALID_SOCKET;
		m_block = false;
	}
	virtual ~LinuxTcpSocketHandler() {
		Shutdown();
		Close();
	}

public:
	// 创建socket
	virtual bool Create()
	{
		if (INVALID_SOCKET == m_socket)
		{
			m_socket = socket(AF_INET, SOCK_STREAM, 0);
		}
		return INVALID_SOCKET != m_socket;
	}
	
	// 停止socket
	virtual void Shutdown()
	{
		if (INVALID_SOCKET != m_socket)
		{
			shutdown(m_socket, SHUT_RDWR);
		}
	}

	// 关闭socket
	virtual void Close()
	{
		if (INVALID_SOCKET != m_socket)
		{
			close(m_socket);
			m_socket = INVALID_SOCKET;

			m_block = false;
		}
	}
	
	// 绑定ip
	virtual bool Bind(const string& ip, unsigned int port)
	{
		bool result = false;
		if (INVALID_SOCKET != m_socket)
		{
			sockaddr_in service;
			service.sin_family = AF_INET;
			service.sin_addr.s_addr = inet_addr(ip.c_str());
			service.sin_port = htons(port);

			result =  SOCKET_ERROR != bind(m_socket, (struct sockaddr*)&service, sizeof(service));
		}
		return result;
	}
	
	// 连接（msTimeout：超时时间(毫秒)，不大于0表示使用默认超时）
	virtual SOCKET_RESULT_CODE Connect(const string& ip, unsigned int port, int msTimeout)
	{
		SOCKET_RESULT_CODE result = SOCKET_RESULT_FAIL;

		// 定义socketaddr
		sockaddr_in server;
		bzero(&server, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons(port);

		if (INVALID_SOCKET != m_socket
			&& !(inet_pton(AF_INET, ip.c_str(), &server.sin_addr) < 0))
		{
			// 连接
			if (msTimeout > 0) {
				SetBlock(false);
				if (connect(m_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
					if (errno == EINPROGRESS) {
						timeval timeout;
						timeout.tv_sec = msTimeout / 1000;
						timeout.tv_usec = msTimeout % 1000;
						fd_set writeset, exceptset;
						FD_ZERO(&writeset);
						FD_SET(m_socket, &writeset);
						FD_ZERO(&exceptset);
						FD_SET(m_socket, &exceptset);

						int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
						if (ret == 0) {
							result = SOCKET_RESULT_TIMEOUT;
						}
						else if (ret > 0 && 0 != FD_ISSET(m_socket, &exceptset)) {
							result  = SOCKET_RESULT_SUCCESS;
						}
					}
					else if (errno == 0) {
						result = SOCKET_RESULT_SUCCESS;
					}
				}
				else {
					result = SOCKET_RESULT_SUCCESS;
				}
				SetBlock(true);
			}
			else {
				SetBlock(true);
				if (connect(m_socket, (struct sockaddr*)&server, sizeof(sockaddr_in)) == 0) {
					result = SOCKET_RESULT_SUCCESS;
				}
			}
		}
		return result;
	}
	
	// 发送
	virtual bool Send(void* data, unsigned int dataLen)
	{
		bool result = false;

		if (INVALID_SOCKET != m_socket)
		{
			int total = 0;
			while (true) {
				int iSent = send(m_socket, (const char*)data, dataLen, 0);
				if (iSent > 0) {
					total += iSent;
				}
				else {
					break;
				}

				if (total == dataLen) {
					result = true;
					break;
				}
			}
		}
		return result;
	}
	
	// 接收
	virtual bool Recv(void* data, unsigned int dataSize, unsigned int& dataLen)
	{
		bool result = false;

		if (INVALID_SOCKET != m_socket)
		{
			dataLen = 0;
			int length = recv(m_socket, (char*)data, dataSize, 0);
			if (length > 0) {
				dataLen = length;
				result = true;
			}
		}

		return result;
	}

private:
	// blocking设置
	bool SetBlock(bool block)
	{
		bool result = false;
		if (INVALID_SOCKET != m_socket)
		{
			result = true;
			if (m_block != block) {
				int flags = fcntl(m_socket, F_GETFL, 0);
				if (block) {
					flags = flags & ~O_NONBLOCK;

				}
				else {
					flags = flags | O_NONBLOCK;
				}

				result = fcntl(m_socket, F_SETFL, flags) == 0;
				if (result) {
					m_block = block;
				}
			}
		}
		return result;
	}

private:
	SOCKET	m_socket;
	bool	m_block;
};

#endif

bool ISocketHandler::InitEnvironment()
{
#ifdef WIN32
	WSADATA wsaData;
	return WSAStartup(MAKEWORD(2,2), &wsaData) == NO_ERROR;
#else
	return true;
#endif
}
	
void ISocketHandler::ReleaseEnvironment()
{
#ifdef WIN32
	WSACleanup();
#else

#endif
}

ISocketHandler* ISocketHandler::CreateSocketHandler(SOCKET_TYPE type)
{
	ISocketHandler* handler = NULL;

	if (TCP_SOCKET == type) {
#ifdef WIN32
		handler = new WinTcpSocketHandler();
#else
		handler = new LinuxTcpSocketHandler();
#endif
	}
	else if (UDP_SOCKET == type) {
		
	}

	return handler;
}
	
void ISocketHandler::ReleaseSocketHandler(ISocketHandler* handler)
{
	delete handler;
}
