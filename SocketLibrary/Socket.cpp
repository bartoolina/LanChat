#include "Socket.h"
#include "Log.h"
#include <assert.h>

namespace SocketLibrary
{
	Socket::Socket(IPVersion ipversion, SOCKET socket)
		:_ipversion(ipversion),_socket(socket)
	{
		assert(_ipversion == IPVersion::IPv4);


	}

	Result Socket::Create()
	{
		assert(_ipversion == IPVersion::IPv4);

		if (_socket != INVALID_SOCKET)
		{
			return Result::Fail;
		}

		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			Log::PrintError("Nie udalo sie utworzyc gniazda", error);
			return Result::Fail;
		}
		
		Log::Print("Gniazdo zostalo utworzone.");

		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) == Result::Success)
		{
			Log::Print("NoDelay zostalo wlaczone na gniezdzie.");
		}
		
		return Result::Success;
	}

	Result Socket::Close()
	{
		if (_socket == INVALID_SOCKET)
		{
			Log::PrintError("Niemozna zamknac nieistniejacego gniazda.", -1);
			return Result::Fail;
		}

		int result = closesocket(_socket);
		if (result != 0)
		{
			int error = WSAGetLastError();
			Log::PrintError("Nie udalo sie zamknac gniazda.", error);
			return Result::Fail;
		}

		_socket = INVALID_SOCKET;
		return Result::Success;
	}

	SOCKET Socket::GetSocket()
	{
		return _socket;
	}

	IPVersion Socket::GetIpVersion()
	{
		return _ipversion;
	}
	Result Socket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
			break;
		default:
			return Result::Fail;
		}

		if (result != 0)
		{
			int error = WSAGetLastError();
			Log::PrintError("Nie mozna ustawic opcji dla gniazda.", error);
			return Result::Fail;
		}

		return Result::Success;
	}
}