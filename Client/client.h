#pragma once
#include <vector>
#include <fstream>
#include <SocketLibrary/SocketLibrary.h>

using namespace SocketLibrary;

class Client
{
	struct clientInfo {
		Socket connection;
		Endpoint listening;
		std::string name;
		time_t lastActiveTime;
	
	};

public:
	Result Initialize(Endpoint ip);
	Result CheckConnection();
	bool isConnectedToSever();
	Result LoginToServer();
	Result SendMsg(std::string name, std::string msg);
	Result SendFile(std::string name, std::string filename);
	Result Input(std::string input);

private:
	Result SendFile();
	Socket listeningSocketTCP;
	Socket broadcastReciverUDP;
	std::vector<clientInfo> connections;
	std::vector<clientInfo> availableClients;

	//	std::vector<Connection> connections;
	fd_set master_fd;
	timeval waitTime = { 0, 1000 };
	Endpoint ownEndpoint;
	std::string fileName;
	std::ofstream writeFile_p;
public:
	std::string ownLogin;
};
