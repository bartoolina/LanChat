#include "Socket.h"
#include "Log.h"
#include <assert.h>

namespace SocketLibrary
{
	Socket::Socket(IPVersion ipversion, Protocol protocol, SOCKET socket)
		:_ipversion(ipversion), _socket(socket), _protocol(protocol)
	{
		assert(_ipversion == IPVersion::IPv4);


	}
	Socket::Socket(Protocol protocol)
		:_ipversion(IPVersion::IPv4), _protocol(protocol), _socket(INVALID_SOCKET)
	{
		assert(_ipversion == IPVersion::IPv4);
	}

	Result Socket::Create()
	{
		assert(_ipversion == IPVersion::IPv4);

		if (_socket != INVALID_SOCKET)
		{
			Log::PrintError("Gniazdo juz istnieje.", -1);
			return Result::Fail;
		}
		else
		{
			Log::Print("Gniazdo nie istnieje. Tworze gniazdo...");
		}

		switch (_protocol)
		{
		case SocketLibrary::Protocol::Unknown:
			Log::PrintError("Nie mozna utworzyc gniazda o nieznamym typie.", -1);
			return Result::Fail;
			break;
		case SocketLibrary::Protocol::TCP:
			_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			break;
		case SocketLibrary::Protocol::UDP:
			_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			break;
		default:
			break;
		}

		if (_socket == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			Log::PrintError("Nie udalo sie utworzyc gniazda", error);
			return Result::Fail;
		}
		else
		{
			Log::Print("Gniazdo zostalo utworzone. [SOCKET:" + std::to_string(_socket) + "]");
		}
		
		switch (_protocol)
		{
		case SocketLibrary::Protocol::TCP:
			if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) == Result::Success)
			{
				Log::Print("NoDelay zostalo wlaczone na gniezdzie TCP.");
			}
			break;
		case SocketLibrary::Protocol::UDP:
			if (SetSocketOption(SocketOption::UDP_Broadcast, TRUE) == Result::Success)
			{
				Log::Print("UDP_Broadcast zostalo wlaczone na gniezdzie UDP.");
			}
			break;
		default:
			break;
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
		else
		{
			Log::Print("Zamykam gniazdo [SOCKET:" + std::to_string(_socket) + "]");
		}

		int result = closesocket(_socket);
		if (result != 0)
		{
			int error = WSAGetLastError();
			Log::PrintError("Nie udalo sie zamknac gniazda.", error);
			return Result::Fail;
		}
		else
		{
			Log::Print("Gniazdo zostalo zamkniete.");
		}

		_socket = INVALID_SOCKET;
		return Result::Success;
	}

	Result Socket::Bind(Endpoint endpoint)
	{
		sockaddr_in addr = endpoint.GetSocketaddrIP();
		int result = bind(_socket, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0)
		{
			int error = WSAGetLastError();
			Log::PrintError("Nie udalo sie powiazac gniazda z portem.", error);
			return Result::Fail;
		}
		else
		{
			Log::Print("Udalo sie zwiazac gniazdo z wybranym portem. [PORT]:" + std::to_string(endpoint.GetPort())
				+ " [SOCKET]:" + std::to_string(_socket));
		}
		
		return Result::Success;
	}

	Result Socket::BindAndListen(Endpoint endpoint, int waitConnection)
	{
		if (Bind(endpoint) != Result::Success)
		{
			return Result::Fail;
		}

		int result = listen(_socket, waitConnection);
		if (result != 0)
		{
			int error = WSAGetLastError();
			Log::PrintError("Nie udalo sie ustawic gniazda w tryb nasluchu", error);
			return Result::Fail;
		}
		else
		{
			Log::Print("Gniazdo jest od teraz w trybie nasluchu.");
		}

		return Result::Success;
	}

	Result Socket::Accpet(Socket& outSocket)
	{
		sockaddr_in addr = {};
		int len = sizeof(sockaddr_in);
		SOCKET acceptedConnection = accept(_socket, (sockaddr*)(&addr), &len);
		if (acceptedConnection == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			Log::PrintError("Wystapil problem przy akceptacji polaczenia.", error);
			return Result::Fail;
		}
		else
		{
			Log::Print("Zaakceptowano nowe poleczenie [SOCKET:" + std::to_string(acceptedConnection) + "]");
		}

		Endpoint newConnectionEndpoint((sockaddr*)&addr);
		newConnectionEndpoint.Print();

		outSocket = Socket(IPVersion::IPv4, Protocol::TCP, acceptedConnection);
		return Result::Success;
	}

	Result Socket::Connect(Endpoint endpoint)
	{
		sockaddr_in addr = endpoint.GetSocketaddrIP();
		int result = connect(_socket, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0)
		{
			int error = WSAGetLastError();
			Log::PrintError("Nie mozna nawiazac polaczenia z " + endpoint.GetIpString(), error);
			return Result::Fail;
		}
		else
		{
			Log::Print("Nawiazano nowe polaczenie z " + endpoint.GetIpString());
		}
		
		return Result::Success;
	}

	Result Socket::SendAll(const void* data, int numberOfBytes)
	{
		int totalBytesSent = 0;
		uint32_t numberBytesToSend = numberOfBytes;
		numberBytesToSend = htonl(numberBytesToSend);
		Result result = Send(&numberBytesToSend, sizeof(uint32_t), totalBytesSent);
		if (totalBytesSent != sizeof(uint32_t))
		{
			Log::PrintError("Nie mozna wyslac dlugosci strumienia danych.", -1);
			return Result::Fail;
		}

		totalBytesSent = 0;

		while (totalBytesSent < numberOfBytes)
		{
			int bytesLeft = numberOfBytes - totalBytesSent;
			int bytesSent = 0;
			char* bufferOffset = (char*)data + totalBytesSent;
			result = Send(bufferOffset, bytesLeft, bytesSent);
			if (result == Result::Fail)
			{
				Log::PrintError("Wyslalem " + std::to_string(totalBytesSent) + " z " + std::to_string(numberOfBytes) + " bajtow.", -1);
				return Result::Fail;
			}
			totalBytesSent += bytesSent;
		}

		return Result::Success;
	}

	Result Socket::ReciveAll(std::string& output, int& numberOfBytes)
	{
		int totalBytesRecive = 0;
		uint32_t numberBytesToRecive = 0;
		Result result = Recive(&numberBytesToRecive, sizeof(uint32_t), totalBytesRecive);
		if (totalBytesRecive != sizeof(uint32_t))
		{
			Log::PrintError("Nie mozna odebrac dlugosci strumienia dancyh.", -1);
			Result::Fail;
		}
		
		totalBytesRecive = 0;
		numberBytesToRecive = ntohl(numberBytesToRecive);
		output.resize(numberBytesToRecive + 1);

		while (totalBytesRecive < numberBytesToRecive)
		{
			int bytesLeft = numberBytesToRecive - totalBytesRecive;
			int bytesRecive = 0;
			char* bufferOffset = &output[0] + totalBytesRecive;
			Result result = Recive(bufferOffset, bytesLeft, bytesRecive);
			if (result == Result::Fail)
			{
				Log::PrintError("Odebralem " + std::to_string(totalBytesRecive) + " z " + std::to_string(numberOfBytes) + " bajtow.", -1);
				return Result::Fail;
			}
			totalBytesRecive += bytesRecive;
		}
		numberOfBytes = totalBytesRecive;

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

	Result Socket::Send(const void* data, int size, int& bytesSent)
	{
		bytesSent = send(_socket, (const char*)data, size, NULL);

		if (bytesSent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			Log::PrintError("Wystapil problem podczas wysylania dancyh.", error);
			return Result::Fail;
		}

		return Result::Success;
	}

	Result Socket::Recive(void* buffor, int size, int& bytesRecived)
	{
		bytesRecived = recv(_socket, (char*)buffor, size, NULL);
		if (bytesRecived == 0)
		{
			Log::PrintError("Polaczenie zostalo zamkniete.", -1);
			// TODO - zamknac gniazdo
			return Result::Fail;
		}
		if (bytesRecived == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			Log::PrintError("Wystapil problem podczas odbierania dancyh.", error);
			return Result::Fail;
		}

		return Result::Success;
	}

	Result Socket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option)
		{
		case SocketOption::TCP_NoDelay:
			result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
			break;
		case SocketOption::UDP_Broadcast:
			result = setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&value, sizeof(value));
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