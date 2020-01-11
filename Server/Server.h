#pragma once
#include <vector>
#include <SocketLibrary/SocketLibrary.h>

using namespace SocketLibrary;

class Server
{
	struct clientInfo {
		Socket connection;
		Endpoint listening;
		std::string name;
		time_t lastActiveTime;
	};
public:
	Result Initialize(Endpoint ip);
	Result IncomingConnection();
	Result CheckConnection();
	Result SendToClients();
private:
	Socket listeningSocketTCP;
	Socket broadcastSenderUDP;
	std::vector<clientInfo> connections;

	fd_set master_fd;
	timeval waitTime = { 0, 1000 };
	Endpoint ownEndpoint;
public:
	bool sendUDP = true;
};