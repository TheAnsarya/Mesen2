#pragma once

#include "pch.h"
#include <mutex>

class Socket
{
private:
	#ifdef _WIN32
	bool _cleanupWSA = false;
	#endif
	
	uintptr_t _socket = (uintptr_t)~0;
	bool _connectionError = false;
	int32_t _UPnPPort = -1;
	
	// Send buffer for BufferedSend/SendBuffer
	std::vector<char> _sendBuffer;
	std::mutex _sendBufferMutex;

public:
	Socket();
	Socket(uintptr_t socket);
	~Socket();

	void SetSocketOptions();
	void SetConnectionErrorFlag();

	void Close();
	bool ConnectionError();

	void Bind(uint16_t port);
	bool Connect(const char* hostname, uint16_t port);
	void Listen(int backlog);
	unique_ptr<Socket> Accept();

	int Send(char *buf, int len, int flags);
	void BufferedSend(char *buf, int len);
	void SendBuffer();
	int Recv(char *buf, int len, int flags);
};
