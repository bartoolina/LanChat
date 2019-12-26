//Client
#include <SocketLibrary/SocketLibrary.h>
//#include <string>
#include <iostream>

using namespace SocketLibrary;

int main()
{
	if (Network::Initialize() == true)
	{
		Log::Print("[CLIENT] Winsok zostal zainicjolizowany. Mozemy dzialac.");
		Socket socket;
		if (socket.Create() == Result::Success)
		{
			Log::Print("[CLIENT] Utworzylem socket.");
			if (socket.Connect(Endpoint("127.0.0.1", 54000)) == Result::Success)
			{
				Log::Print("[CLIENT] Polaczylem sie serverem.");
				std::string outString = "Wysylam z klienta.";
				char buffer[256];
				strcpy_s(buffer, "witam z klienta!\0");
				
				while (true)
				{
					socket.SendAll(outString.data(), outString.size());
					//socket.SendAll(buffer, 256);
					std::cout << "wyslalem 256 bajtow" << std::endl;
					Sleep(1000);
				}
			}
			
			socket.Close();
		}
	}


	Network::Shutdown();
	system("pause");
	return 0;
}


//(VC++ Directories) Add Include Dir -> Solution Dir
//(VC++ Directories) Add Library Dir -> Output Dir
//(Linket/Input) Add Additional Dep -> SocketLibrary.lib
//(Solution/Project Build Order/Dependecies) Correct Oreder Build
//(Linker/Input) Add Additional Dep -> ws2_32.lib