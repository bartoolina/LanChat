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

