#include <iostream>
#include "Log.h"
#include "Result.h"
#include "ipVersion.h"

void SocketLibrary::Log::Print(std::string message)
{
	std::cout << "[LOG] " << message << std::endl;
}

void SocketLibrary::Log::PrintError(std::string message, int error)
{
	std::cerr << "[ERROR] " << "[Error Code: " << error << "] "<< message << std::endl;
}
