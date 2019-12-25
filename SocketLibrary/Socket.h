#pragma once
#include <WinSock2.h>

#include "ipVersion.h"
#include "Result.h"
#include "SocketOption.h"

namespace SocketLibrary
{
	class Socket
	{
	public:
		Socket(IPVersion ipversion = IPVersion::IPv4, SOCKET socket = INVALID_SOCKET); //(..., SocketHandle handle)
		Result Create();
		Result Close();
		SOCKET GetSocket();		//SocketHandle GetHandle()
		IPVersion GetIpVersion();
	private:
		Result SetSocketOption(SocketOption option, BOOL value);
		IPVersion _ipversion = IPVersion::IPv4;
		SOCKET _socket = INVALID_SOCKET;
		
	};
}