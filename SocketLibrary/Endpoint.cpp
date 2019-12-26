#include "Endpoint.h"
//#include <WS2tcpip.h>
#include "Log.h"
#include <assert.h>

using namespace SocketLibrary;

Endpoint::Endpoint(const char* ip, unsigned short port)
{
	this->_port = port;

	in_addr addr;
	int result = inet_pton(AF_INET, ip, &addr);

	if (result == 1)
	{
		if (addr.S_un.S_addr != INADDR_NONE)
		{
			_ip_string = ip;
			_hostname = ip;
			_ipversion = IPVersion::IPv4;

			_socketaddr.sin_family = AF_INET;
			_socketaddr.sin_port = htons(_port);
			memcpy(&_socketaddr.sin_addr, &addr, sizeof(in_addr));
			
			return;
		}
	}
	else
	{
		std::string invalidIP = ip;
		Log::PrintError("To nie jest prawidlowy adres IP. [" + invalidIP + "]", result);
	}
}

Endpoint::Endpoint(sockaddr* addr)
{
	assert(addr->sa_family == AF_INET);
	sockaddr_in* newaddr = (sockaddr_in*)addr;
	_ipversion = IPVersion::IPv4;
	_port = ntohs(newaddr->sin_port);
	_ip_string.resize(16);
	inet_ntop(AF_INET, &newaddr->sin_addr, &_ip_string[0], 16);
	_hostname = _ip_string;
}

IPVersion Endpoint::GetIpVersion()
{
	return _ipversion;
}

std::string Endpoint::GetHostName()
{
	return _hostname;
}

std::string Endpoint::GetIpString()
{
	return _ip_string;
}

unsigned short Endpoint::GetPort()
{
	return _port;
}

sockaddr_in Endpoint::GetSocketaddrIP()
{
	return _socketaddr;
}

void SocketLibrary::Endpoint::Print()
{
	Log::Print("Hostname: " + _hostname);
	Log::Print("IP: " + _ip_string);
	Log::Print("Port: " + std::to_string(_port));
	Log::Print("Family: " + std::to_string(_socketaddr.sin_family));
}
