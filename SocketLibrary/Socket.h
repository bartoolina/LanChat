#pragma once
#include <WinSock2.h>
#include <vector>
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
		Socket(SOCKET socket);
		Result Create();
		Result Close();
		Result AssignEndpoint(Endpoint endpoint);
		Result Bind(Endpoint endpoint);
		Result BindAndListen(Endpoint endpoint, int waitConnection = 5);
		Result Accept(Socket& outSocket);
		Result Connect();
		Result Connect(Endpoint endpoint);
		Result SendUDP(std::string& listOfUsers, Endpoint server);
		Result ReciveUDP(std::string& output, int& numberOfBytes);
		Result SendAll(const void * data, int numberOfBytes);
		Result ReciveAll(std::string& output, int& numberOfBytes);
		SOCKET GetSocket();		//SocketHandle GetHandle()
		IPVersion GetIpVersion();
		Endpoint GetEndpoint();
	private:
		Result Send(const void * data, int size, int& bytesSent);
		Result Recive(void * buffor, int size, int & bytesRecived);
		Result SetSocketOption(SocketOption option, BOOL value);
		IPVersion _ipversion = IPVersion::IPv4;
		Protocol _protocol = Protocol::TCP;
		SOCKET _socket = INVALID_SOCKET;
		Endpoint _endpoint = {};
		
	};
}