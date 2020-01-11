#pragma once
#include <vector>
#include "Socket.h"
#include "Endpoint.h"

namespace SocketLibrary
{
	class Connection
	{
		Connection(Socket socket, Endpoint endpoint);

		Endpoint endpoint;
		Socket socket;
	};
}