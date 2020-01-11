#pragma once
#include <string>
#include "Result.h"
namespace SocketLibrary
{
	enum class TypeOfMsg
	{
		Unknown,
		Text,
		File,
		Command
	};

	enum class Command
	{
		Unknown,
		LogIn,
		LogOut,
		Adress,
		ConnCheck,
		FileName
	};

	class Message
	{
	public:
		Message();


		Result Parse(const std::string& msg, int size);
		std::string PrepareText(const std::string& msg, std::string name);
		std::string PrepareFile(const std::string& msg, std::string name, int totalLenght);
		std::string PrepareCommand(const std::string& msg, std::string name, Command cmd);
		std::string GetName();
		TypeOfMsg GetType();
		int GetLenght();
		int GetTotalLenght();
		std::string  GetMsg();
		Command GetCommand();
		std::string GetFileName();
	private:
		std::string _name;
		TypeOfMsg _typeOfMsg;
		int _lenght;
		int _totalLenght;
		std::string _message;
		Command _command;
		std::string _filename;

	};
}