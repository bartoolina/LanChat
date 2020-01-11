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




//using namespace SocketLibrary;
//
//int main()
//{
//	if (Network::Initialize() == true)
//	{
//		Log::Print("[SERVER] Winsok zostal zainicjolizowany. Mozemy dzialac.");
//
//
//		Socket socketTCP(Protocol::TCP);
//		Socket socketUDP(Protocol::UDP);
//		if (socketTCP.Create() == Result::Success)
//		{
//			socketUDP.Create();
//			socketUDP.Bind(Endpoint("0.0.0.0", 54001));
//			Log::Print("[SERVER] Utworzylem gniazdo.");
//			if (socketTCP.BindAndListen(Endpoint("127.0.0.1", 54000), 5) == Result::Success)
//			{
//
//
//
//				Socket newConnection;
//
//
//
//				Log::Print("[SERVER] sukces");
//
//
//				if (socketTCP.Accept(newConnection) == Result::Success)
//				{
//					Log::Print("[SERVER] accept: sukces");
//					std::string buffer = "";
//					int size = 0;
//					while (true)
//					{
//
//						newConnection.ReciveAll(buffer, size);
//						std::cout << "[" << size << "] " << buffer << std::endl;
//						buffer = "";
//					}
//					newConnection.Close();
//				}
//				else
//				{
//					Log::PrintError("[SERVER] accept: porazka", -1);
//				}
//			}
//			socketTCP.Close();
//		}
//		else
//		{
//			Log::PrintError("[SERVER] porazka", -1);
//		}
//	}
//
//
//	Network::Shutdown();
//	return 0;
//}
