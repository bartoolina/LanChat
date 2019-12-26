#pragma once
#include <WinSock2.h>

#include "ipVersion.h"
#include "Protocol.h"
#include "Result.h"
#include "SocketOption.h"
#include "Endpoint.h"

namespace SocketLibrary
{
	class Socket
	{
	public:
		Socket(IPVersion ipversion = IPVersion::IPv4, Protocol protocol = Protocol::TCP, SOCKET socket = INVALID_SOCKET); //(..., SocketHandle handle)
		Socket(Protocol protocol);
		Result Create();
		Result Close();
		Result Bind(Endpoint endpoint);
		Result BindAndListen(Endpoint endpoint, int waitConnection = 5);
		Result Accpet(Socket& outSocket);
		Result Connect(Endpoint endpoint);
		Result SendAll(const void * data, int numberOfBytes);
		Result ReciveAll(std::string& output, int& numberOfBytes);
		SOCKET GetSocket();		//SocketHandle GetHandle()
		IPVersion GetIpVersion();
	private:
		Result Send(const void * data, int size, int& bytesSent);
		Result Recive(void * buffor, int size, int & bytesRecived);
		Result SetSocketOption(SocketOption option, BOOL value);
		IPVersion _ipversion = IPVersion::IPv4;
		Protocol _protocol = Protocol::TCP;
		SOCKET _socket = INVALID_SOCKET;
		
	};
}