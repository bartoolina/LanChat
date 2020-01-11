#include "Connection.h"

SocketLibrary::Connection::Connection(Socket socket, Endpoint endpoint)
	:socket(socket), endpoint(endpoint)
{
}
