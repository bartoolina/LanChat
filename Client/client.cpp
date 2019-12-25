//Client
#include <SocketLibrary/SocketLibrary.h>

int main()
{
	int value = SocketLibrary::ReturnFive();
	return 0;
}


//(VC++ Directories) Add Include Dir -> Solution Dir
//(VC++ Directories) Add Library Dir -> Output Dir
//(Linket/Input) Add Additional Dep -> SocketLibrary.lib
//(Solution/Project Build Order/Dependecies) Correct Oreder Build