//Server
#include <SocketLibrary/SocketLibrary.h>

using namespace SocketLibrary;

int main()
{
	if (Network::Initialize() == true)
	{
		Log::Print("[SERVER] Winsok zostal zainicjolizowany. Mozemy dzialac.");
	}

	
	Network::Shutdown();
	return 0;
}

//(VC++ Directories) Add Include Dir -> Solution Dir
//(VC++ Directories) Add Library Dir -> Output Dir
//(Linket/Input) Add Additional Dep -> SocketLibrary.lib
//(Solution/Project Build Order/Dependecies) Correct Oreder Build
//(Linker/Input) Add Additional Dep -> ws2_32.lib