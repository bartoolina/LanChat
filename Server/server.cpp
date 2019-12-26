//Server
#include <SocketLibrary/SocketLibrary.h>
#include <iostream>
using namespace SocketLibrary;

int main()
{
	if (Network::Initialize() == true)
	{
		Log::Print("[SERVER] Winsok zostal zainicjolizowany. Mozemy dzialac.");

		Socket socketTCP(Protocol::TCP);
		Socket socketUDP(Protocol::UDP);
		if (socketTCP.Create() == Result::Success)
		{
			socketUDP.Create();
			socketUDP.Bind(Endpoint("0.0.0.0", 54001));
			Log::Print("[SERVER] Utworzylem gniazdo.");
			if (socketTCP.BindAndListen(Endpoint("127.0.0.1", 54000), 5) == Result::Success)
			{
				Log::Print("[SERVER] sukces");
				Socket newConnection;
				if (socketTCP.Accpet(newConnection) == Result::Success)
				{
					Log::Print("[SERVER] accept: sukces");
					std::string buffer = "";
					int size = 0;
					while (true)
					{
						newConnection.ReciveAll(buffer, size);
						std::cout << "[" << size << "] "<< buffer << std::endl;
						buffer = "";
					}
					newConnection.Close();
				}
				else
				{
					Log::PrintError("[SERVER] accept: porazka", -1);
				}
			}
			socketTCP.Close();
		}
		else
		{
			Log::PrintError("[SERVER] porazka", -1);
		}
	}

	
	Network::Shutdown();
	return 0;
}

//(VC++ Directories) Add Include Dir -> Solution Dir
//(VC++ Directories) Add Library Dir -> Output Dir
//(Linket/Input) Add Additional Dep -> SocketLibrary.lib
//(Solution/Project Build Order/Dependecies) Correct Oreder Build
//(Linker/Input) Add Additional Dep -> ws2_32.lib