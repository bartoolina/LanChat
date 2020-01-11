#include "client.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <chrono>
#include <future>

std::string GetLineFromCin() {
	std::string line;
	std::getline(std::cin, line);
	return line;
}


int main()
{
	srand(time(NULL));
	unsigned short randomPort = rand() % 100 + 52000;
	Client client;

	auto future = std::async(std::launch::async, GetLineFromCin);

	time_t last_time = time(NULL);

	if (client.Initialize(Endpoint("127.0.0.1", randomPort)) == Result::Success)
	{
		while (true)
		{
			client.CheckConnection();
			if (client.isConnectedToSever())
			{
				if (client.ownLogin == "")
				{
					std::cout << "Podaj Login: ";
					//std::getline(std::cin, client.ownLogin);
					client.ownLogin = future.get();
					client.LoginToServer();
					future = std::async(std::launch::async, GetLineFromCin);
				}
				

				if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) 
				{
					auto line = future.get();
					client.Input(line);
					future = std::async(std::launch::async, GetLineFromCin);
				}
			}
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
//		Log::Print("[CLIENT] Winsok zostal zainicjolizowany. Mozemy dzialac.");
//		Socket socket;
//		if (socket.Create() == Result::Success)
//		{
//			Log::Print("[CLIENT] Utworzylem socket.");
//			if (socket.Connect(Endpoint("127.0.0.1", 53000)) == Result::Success)
//			{
//				Log::Print("[CLIENT] Polaczylem sie serverem.");
//				std::string outString = "Wysylam z klienta.";
//				char buffer[256];
//				strcpy_s(buffer, "witam z klienta!\0");
//				char c = NULL;
//				while (true)
//				{
//					socket.SendAll(outString.data(), outString.size());
//					//socket.SendAll(buffer, 256);
//					std::cout << "wyslalem: " << outString << std::endl;
//					std::string inString = "";
//					int bytesRecived = 0;
//					socket.ReciveAll(inString, bytesRecived);
//					std::cout << "Odebralem [" << bytesRecived << "]: " << inString << std::endl;
//					//char d = std::getchar();
//					/*if (d == 'q')
//					{
//						break;
//					}*/
//					Sleep(1000);
//				}
//			}
//
//			socket.Close();
//		}
//	}
//
//
//	Network::Shutdown();
//	system("pause");
//	return 0;
//}