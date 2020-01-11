#include <sstream>

#include "Message.h"

namespace SocketLibrary
{
	Message::Message()		
	{
		_name = "";
		_typeOfMsg = TypeOfMsg::Unknown;
		_lenght = 0;
		_totalLenght = 0;
		_message = {};
		_command = Command::Unknown;
		_filename = "";
	}

	Result Message::Parse(const std::string& msg, int lenght)
	{
		std::string expression, left, right;
		std::stringstream msgStream;
		msgStream << msg;
		
		for (int i = 0; i < 4; i++)
		{
			//std::getline(msgStream, expression);
			msgStream >> expression;

			expression.replace(expression.find(':'), 1, " ");
			std::stringstream expStream(expression);
			
			expStream >> left >> right;
			if (i == 0)
			{
				if (left == "who")	
					_name = right;
				else return Result::Fail;

			}
			else if (i == 1)
			{
				if (left == "what")
				{
					if (right == "text")
						_typeOfMsg = TypeOfMsg::Text;
					else if (right == "file")
						_typeOfMsg = TypeOfMsg::File;
					else if (right == "command")
						_typeOfMsg = TypeOfMsg::Command;
					else return Result::Fail;		
				}
				else return Result::Fail;
			}
			else if (i == 2)
			{
				if (left == "sizetotal")	_totalLenght = atoi(right.c_str());
				else return Result::Fail;
			}
			else if (i == 3)
			{
				if (left == "lenght")	_lenght = atoi(right.c_str());
				else return Result::Fail;
			}
		}

		size_t pos = msgStream.tellg();
		pos++;
		_message = msg.substr(pos);

		if (_lenght != _message.size())
		{
			return Result::Fail;
		}


		if (_typeOfMsg == TypeOfMsg::Command)
		{
			std::size_t pos = _message.find(':');
			std::string cmd = _message.substr(0, pos);
			if (cmd == "login")
			{
				_command = Command::LogIn;
				//_message = _message.substr(_message.find(':') + 1);
			}
			else if (cmd == "logout")
			{
				_command = Command::LogOut;
				//_message = _message.substr(_message.find(':') + 1);
			}
			else if (cmd == "adress")
			{
				_command = Command::Adress;
				_message = _message.substr(_message.find(':') + 1);
			}
			else if (cmd == "conn_check")
			{
				_command = Command::ConnCheck;
			}
			else if (cmd == "filename")
			{
				_command = Command::FileName;
				_filename = _message.substr(_message.find(':') + 1);
			}
			else return Result::Fail;
		}
		
		return Result::Success;
	}

	std::string Message::PrepareText(const std::string& msg, std::string name)
	{
		//"who:bart what:text sizetotal:10 lenght:10 tresc wiadomosci i tak dalej";
		int lenght = msg.size();
		std::string output;
		output = "who:" + name;
		output += " what:text";
		output += " sizetotal:" + std::to_string(lenght);
		output += " lenght:" + std::to_string(lenght);
		output += " " + msg;

		return output;
	}

	std::string Message::PrepareFile(const std::string& msg, std::string name, int totalLenght)
	{
		//"who:bart what:file sizetotal:1000 lenght:10 ......plik.....binary......";
		int lenght = msg.size();
		std::string output;
		output = "who:" + name;
		output += " what:file";
		output += " sizetotal:" + std::to_string(totalLenght);
		output += " lenght:" + std::to_string(lenght);
		output += " " + msg;

		return output;
		
	}

	std::string Message::PrepareCommand(const std::string& msg, std::string name, Command cmd)
	{
		std::string msgCmd;
		switch (cmd)
		{
		case SocketLibrary::Command::LogIn:
			msgCmd = " login:";// +name;
			break;
		case SocketLibrary::Command::LogOut:
			msgCmd = " logout:";// +name;
			break;
		case SocketLibrary::Command::Adress:
			msgCmd = " adress:" + msg;
			break;
		case SocketLibrary::Command::ConnCheck:
			msgCmd = " conn_check";
			break;
		case Command::FileName:
			msgCmd = " filename:" + msg;
			break;
		default:
			break;
		}	
		//msgCmd += msg;

		int lenght = msgCmd.size()-1;
		std::string output;
		output = "who:" + name;
		output += " what:command";
		output += " sizetotal:" + std::to_string(lenght);
		output += " lenght:" + std::to_string(lenght);
		output += msgCmd;


		return output;
	}

	std::string Message::GetName()
	{
		return _name;
	}

	TypeOfMsg Message::GetType()
	{
		return _typeOfMsg;
	}

	int Message::GetLenght()
	{
		return _lenght;
	}

	int Message::GetTotalLenght()
	{
		return _totalLenght;
	}

	std::string Message::GetMsg()
	{
		return _message;
	}
	Command Message::GetCommand()
	{
		return _command;
	}
	std::string Message::GetFileName()
	{
		return _filename;
	}
}