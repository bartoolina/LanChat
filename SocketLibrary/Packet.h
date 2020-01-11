#pragma once
#include <string>

#include "Result.h"
#include "Endpoint.h"
#include "Socket.h"

namespace SocketLibrary
{
	enum class TypeOfMessage
	{
		Text,
		File,
		Command

	};

	class Packet
	{
		//Result ResolveServer(std::string input, Endpoint& server);


		//Result SendMsg(std::string message, Socket client);
		//Result SendFile(std::string file, Socket client);

		std::string sender;
		TypeOfMessage typeOfMessage;
		int totalLenght;
		int msgLenght;
		std::string message;

	};
}