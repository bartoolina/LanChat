#include <ctime>
#include "Server.h"

using namespace SocketLibrary;

int main()
{
	Server server;
	server.Initialize(Endpoint("127.0.0.1", 53000));

	
	time_t last_time = time(NULL);

	while (true)
	{
		server.IncomingConnection();
		if (last_time + 10 < time(NULL))
		{
			server.CheckConnection();
			last_time = time(NULL);
		}
		
		
	}
	return 0;
}


