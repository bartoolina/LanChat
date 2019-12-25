#pragma once
#include <string>

namespace SocketLibrary
{
	class Log
	{
	public:
		void static Print(std::string message);
		void static PrintError(std::string message, int error);
	};
}