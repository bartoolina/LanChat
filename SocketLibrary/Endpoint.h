#pragma once
#include <string>
#include <WS2tcpip.h>
#include "ipVersion.h"

namespace SocketLibrary
{
	class Endpoint
	{
	public:
		Endpoint(const char* ip, unsigned short port);
		Endpoint(sockaddr* addr);
		IPVersion GetIpVersion();
		std::string GetHostName();
		std::string GetIpString();
		unsigned short GetPort();
		sockaddr_in GetSocketaddrIP();
		void Print();
	private:
		IPVersion _ipversion = IPVersion::Unknown;
		std::string _hostname = "";
		std::string _ip_string = "";
		unsigned short _port = 0;
		sockaddr_in _socketaddr = {};
	};
}