#include <WinSock2.h>

#include "Network.h"
#include "Log.h"

bool SocketLibrary::Network::Initialize()
{
	WSADATA wsadata;
	
	int result = WSAStartup(MAKEWORD(2,2), &wsadata);
	if (result != 0)
	{
		Log::PrintError("Nie udalo sie uruchomic Winsock", result);
		return false;
	}
	else
	{
		Log::Print("Winsock zostal uruchominy");
	}

	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
	{
		Log::PrintError("Nie udalo sie uruchomic WinSock w najwyzszej wersji.", result);
		return false;
	}
	else
	{
		Log::Print("Udalo sie uchomic WinSock w wersji 2,2.");
	}

	return true;
}

void SocketLibrary::Network::Shutdown()
{
	WSACleanup();
}
