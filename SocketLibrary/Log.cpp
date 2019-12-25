#include <iostream>
#include "Log.h"

void SocketLibrary::Log::Print(std::string message)
{
	std::cout << message << std::endl;
}

void SocketLibrary::Log::PrintError(std::string message, int error)
{
	std::cerr << "[Error Code: " << error << "] "<< message << std::endl;
}
