#include "Endpoint.h"
//#include <WS2tcpip.h>
#include "Log.h"
#include <assert.h>

using namespace SocketLibrary;

Endpoint::Endpoint()
{
	char ip[16];
	sockaddr_in newaddr;
	newaddr.sin_family = AF_INET;
	newaddr.sin_port = 0;
	newaddr.sin_addr.S_un.S_addr = INADDR_NONE;

//	sockaddr_in* newaddr;
	_ipversion = IPVersion::IPv4;
	_port = ntohs(newaddr.sin_port);
	//_ip_string.resize(16);
	inet_ntop(AF_INET, &newaddr.sin_addr, /*&_ip_string[0]*/ ip, 16);
	//_ip_string.erase(_ip_string.find('\0'));
	_ip_string = ip;
	_hostname = _ip_string;

	_socketaddr = newaddr;
}

Endpoint::Endpoint(const char* ip, unsigned short port)
{
	_port = port;

	in_addr addr;
	int result = inet_pton(AF_INET, ip, &addr);

	if (result == 1)
	{
//		if (addr.S_un.S_addr != INADDR_NONE)
//		{
			_ip_string = ip;
			_hostname = ip;
			_ipversion = IPVersion::IPv4;

			_socketaddr.sin_family = AF_INET;
			_socketaddr.sin_port = htons(_port);
			memcpy(&_socketaddr.sin_addr, &addr, sizeof(in_addr));
			
			return;
//		}
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
	char ip[16];
	sockaddr_in* newaddr = (sockaddr_in*)addr;
	_ipversion = IPVersion::IPv4;
	_port = ntohs(newaddr->sin_port);
	//_ip_string.resize(16);
	inet_ntop(AF_INET, &newaddr->sin_addr, ip, 16);
	_ip_string = ip;
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
