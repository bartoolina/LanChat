//client
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <ctime>
#include <fstream>
#include <iomanip>
#include "client.h"

Result Client::Initialize(Endpoint ip)
{
	connections.clear();
	FD_ZERO(&master_fd);

	if (Network::Initialize())
	{
		// Winsock dziala
		std::cout << "[CLIENT] Winsock zostal poprawnie zainicjalizowany." << std::endl;

		broadcastReciverUDP = Socket(Protocol::UDP_Reciver);
		if (broadcastReciverUDP.Create() == Result::Success)
		{
			// Socket do odebrania adresu serwara zostal utworzony
			std::cout << "[CLIENT] Gniazdo UDP do odnalezienia serwara zostalo utworzone." << std::endl;
			
			if (broadcastReciverUDP.Bind(Endpoint("0.0.0.0", 54000)) == Result::Success)
			{
				// Socket UDP przypiety do portu
				std::cout << "[CLIENT] Gniazdo UDP zostalo przypisane do portu 54000" << std::endl;
				// Dodanie socketa do listy
				FD_SET(broadcastReciverUDP.GetSocket(), &master_fd);
			}
			else
			{

				std::cerr << "[CLIENT] Nie udalo sie przypisac portu dla broadcastu UDP." << std::endl;
				broadcastReciverUDP.Close();
				return Result::Fail;
			}
		}
		else
		{
			std::cout << "[CLIENT] Nie moge utworzyc gniazda UDP." << std::endl;
			return Result::Fail;
		}

		listeningSocketTCP = Socket(Protocol::TCP);
		if (listeningSocketTCP.Create() == Result::Success)
		{
			//  Socket listening zostal utworzony
			std::cout << "[CLIENT] Gniazdo TCP do nasluchu zostalo utworzone." << std::endl;

			if (listeningSocketTCP.BindAndListen(ip) == Result::Success)
			{
				// Socket listening od teraz nasluchuje
				std::cout << "[SERVER] Gniazdo TCP do nasluchu nasluchuje." << std::endl;
				// Dodanie socketa do listy 
				FD_SET(listeningSocketTCP.GetSocket(), &master_fd);

			}
			else
			{
				std::cerr << "[CLIENT] Nie udalo sie wlaczyc nasluchu na gniezdie TCP." << std::endl;
				listeningSocketTCP.Close();
				return Result::Fail;
			}
		}
		else
		{
			std::cerr << "[CLIENT] Nie mozna utworzych gniazda TCP do nasluchu." << std::endl;
			return Result::Fail;
		}
		return Result::Success;
	}
}

Result Client::CheckConnection()
{
	fd_set copy_fd = master_fd;

	int ConnectionCount = select(NULL, &copy_fd, NULL, NULL, &waitTime);
	if (ConnectionCount)
	{
		if (ConnectionCount == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			Log::PrintError("[SERVER] Problem z listening socket", error);
			return Result::Fail;
		}

		for (int i = 0; i < ConnectionCount; i++)
		{
			if (copy_fd.fd_array[i] == listeningSocketTCP.GetSocket())
			{
				Socket newConnectionSocket(Protocol::TCP);
				//Endpoint newConnectionEndpoint();
				if (listeningSocketTCP.Accept(newConnectionSocket) == Result::Success)
				{
					struct clientInfo newClient;
					newClient.connection = newConnectionSocket;
					newClient.lastActiveTime = time(NULL);
					connections.push_back(newClient);
					//connections.push_back({newConnectionSocket, NULL, NULL, time(NULL)});
					FD_SET(newConnectionSocket.GetSocket(), &master_fd);
					//continue;
				}
				else
				{
					//error
				}
			}

			else if (copy_fd.fd_array[i] == broadcastReciverUDP.GetSocket())
			{
				std::string msg = "";
				int bytes = 0;
				broadcastReciverUDP.ReciveUDP(msg, bytes);
				//std::cout << "odebrano wiadosmosc broadcast o tresci: " << msg << std::endl;
			
				std::string name, adress;
				unsigned short port;

				std::replace(msg.begin(), msg.end(), ':', ' ');
				std::stringstream msgToParse(msg);

				msgToParse >> name >> adress >> port;
				auto server = std::find_if(connections.begin(), connections.end(), [](clientInfo clientInfo) { return  clientInfo.name == "server"; });
				if (server == connections.end()) 
				{
					Socket serverSocket(Protocol::TCP);
					Endpoint serverEnpoint(adress.c_str(), port);
					if (serverSocket.Create() == Result::Success)
					{
					
						if (serverSocket.Connect(serverEnpoint) == Result::Success)
						{
							struct clientInfo newServer;
							newServer.connection = serverSocket;
							newServer.name = "server";
							newServer.listening = serverEnpoint;
							newServer.lastActiveTime = time(NULL);
							connections.push_back(newServer);
							FD_SET(serverSocket.GetSocket(), &master_fd);
						}
					}

				}
			}

			else
			{
				Message recived;
				std::string recivedMessage;
				int bytesRecived;
				
				SOCKET lookingFor = copy_fd.fd_array[i];
				auto findedClient = std::find_if(connections.begin(), connections.end(), [lookingFor](clientInfo client) { return lookingFor == client.connection.GetSocket(); });
				
				switch (findedClient->connection.ReciveAll(recivedMessage, bytesRecived))
				{
				case Result::Fail:
				case Result::ConnectionLost:
					FD_CLR(findedClient->connection.GetSocket(), &master_fd);
					findedClient->connection.Close();
					connections.erase(findedClient);
					break;

				case Result::Success:
					//std::cout << "[" << bytesRecived << "] "<< recivedMessage << std::endl;

					findedClient->lastActiveTime = time(NULL);
					if (recived.Parse(recivedMessage, bytesRecived) != Result::Success)
					{
						// wiadamosc niekompletna, bledna lub cos takiego
						if (writeFile_p.is_open())
						{
							writeFile_p.close();
						}
						return Result::Fail;
					}

					switch (recived.GetType())
					{
					case TypeOfMsg::Unknown:
						break;
					case TypeOfMsg::Text:
					{
						std::string name = recived.GetName();
						std::string msg = recived.GetMsg();
						if (findedClient->name != "server")
						{
							std::cout << "<" << name << "> " << msg << std::endl;
						}
						else
						{
							std::cout << "[" <<  name << "] " << msg << std::endl;
						}
						break;

					}
					case TypeOfMsg::File:
					{
						
						//std::ofstream writeFile(fileName, std::ios::binary | std::ios::trunc);
						if (!writeFile_p)
						{
							std::cout << "Nie udalo sie utworzyc pliku: " << fileName << std::endl;
							writeFile_p.close();
						}
						else
						{
							size_t totalSize = recived.GetTotalLenght();
							size_t recivedBytes = recived.GetLenght();
							writeFile_p.write(recived.GetMsg().c_str(), recivedBytes);
							size_t actualSize = writeFile_p.tellp();
							float percent = ((float)actualSize / totalSize);
							
							int barWidth = 60;
							std::cout << "[";
							int pos = barWidth * percent;
							for (int i = 0; i < barWidth; ++i) {
								if (i < pos) std::cout << "=";
								else if (i == pos) std::cout << ">";
								else std::cout << " ";
							}
							std::cout << "] " << int(percent * 100.0) << " " <<std::setw(3) << "% " << actualSize << '\r';
							std::cout.flush();
							if (totalSize == actualSize)
							{
								writeFile_p.close();
								std::cout << std::endl;
								std::cout << "Odebralem plik " << fileName << std::endl;
							}

						}
						
						break;

					}
					case TypeOfMsg::Command:

						switch (recived.GetCommand())
						{
						case Command::LogIn:
						{
							findedClient->name = recived.GetName();
							break;
						}
						case Command::LogOut:
						{
							std::string nameLogOut = recived.GetName();
							std::cout << nameLogOut << " wylogowal sie." << std::endl;
							auto connToDelete = std::find_if(connections.begin(), connections.end(), [nameLogOut](clientInfo client) {return nameLogOut == client.name; });
							if (connToDelete != connections.end())
							{	
								FD_CLR(connToDelete->connection.GetSocket(), &master_fd);
								connToDelete->connection.Close();
								
								connections.erase(connToDelete);
							}
							
							auto availableClientToDelete = std::find_if(availableClients.begin(), availableClients.end(), [nameLogOut](clientInfo client) {return nameLogOut == client.name; });
							if (availableClientToDelete != availableClients.end())
							{	
								availableClients.erase(availableClientToDelete);
							}
							break; 
						}
						case Command::Adress:
						{
							int port = atoi(recived.GetMsg().c_str());
							Endpoint clientPortListenning = Endpoint(findedClient->connection.GetEndpoint().GetIpString().c_str(), (unsigned short)port);
							struct clientInfo newClient;
							newClient.listening = clientPortListenning;
							newClient.name = recived.GetName();
							availableClients.push_back(newClient);
							std::cout << newClient.name << " jest dostepny." << std::endl;
						}
							break;
						case Command::ConnCheck:
						{
							Message msgConnCheck;
							std::string msg = msgConnCheck.PrepareCommand("", ownLogin, Command::ConnCheck);
							if (findedClient->connection.SendAll(msg.c_str(), msg.size()) != Result::Success)
							{
								//cos poszlo nie tak
							}
							break;

						}
						case Command::FileName:
						{
							fileName = recived.GetFileName();
							fileName.insert(fileName.find('.'), "_reciv");

							std::cout << "Uzytkownik " << recived.GetName() << " wysyla Ci plik " << fileName << std::endl;

							writeFile_p = std::ofstream();
							writeFile_p.open(fileName, std::ios::binary | std::ios::trunc);
						}
						default:
							break;
						}

						break;
					default:
						break;
					}
				default:
					break;
				}


			}



		}
	}

}

bool Client::isConnectedToSever()
{
	if (connections.size() > 0)
	{
		return true;
	}
	return false;
}

Result Client::LoginToServer()
{
	auto server = std::find_if(connections.begin(), connections.end(), [](clientInfo conn) { return conn.name == "server"; });
	if (server != connections.end())
	{
		Message addres;
		std::string port = std::to_string(listeningSocketTCP.GetEndpoint().GetPort());
		std::string msgAddres = addres.PrepareCommand(port, ownLogin, Command::Adress);
		if (server->connection.SendAll(msgAddres.c_str(), msgAddres.size()) != Result::Success)
		{
			// cos poszlo nie tak
			return Result::Fail;
		}

		Message login;
		std::string msgLogin = login.PrepareCommand("", ownLogin, Command::LogIn);
		if (server->connection.SendAll(msgLogin.c_str(), msgLogin.size()) != Result::Success)
		{
			// cos poszlo nie tak
			return Result::Fail;
		}

	}
	return Result::Success;
}

Result Client::SendMsg(std::string name, std::string msg)
{
	Message msgToUser;
	std::string msgToSend = msgToUser.PrepareText(msg, ownLogin);

	auto user = std::find_if(connections.begin(), connections.end(), [name](clientInfo client) {return client.name == name; });
	if (user != connections.end())
	{
		
		user->connection.SendAll(msgToSend.c_str(), msgToSend.size());
	}
	else
	{
		auto userNotConn = std::find_if(availableClients.begin(), availableClients.end(), [name](clientInfo client) {return client.name == name; });
		if (userNotConn != availableClients.end())
		{
			Socket newClientSocket(Protocol::TCP);
			if (newClientSocket.Create() == Result::Success)
			{
				if (newClientSocket.Connect(userNotConn->listening) == Result::Success)
				{
					userNotConn->connection = newClientSocket;
					
					connections.push_back(*userNotConn);
					FD_SET(newClientSocket.GetSocket(), &master_fd);

					Message login;
					std::string msgLogin = login.PrepareCommand("", ownLogin, Command::LogIn);
					if (userNotConn->connection.SendAll(msgLogin.c_str(), msgLogin.size()) != Result::Success)
					{
						// cos poszlo nie tak
						return Result::Fail;
					}
					else
					{
						userNotConn->connection.SendAll(msgToSend.c_str(), msgToSend.size());

					}
				}
			}
		}
	}
	return Result::Success;
}

Result Client::SendFile(std::string name, std::string filename)
{
	auto user = std::find_if(connections.begin(), connections.end(), [name](clientInfo client) {return client.name == name; });

	std::ifstream readFile("C:/test/"+filename, std::ios::binary | std::ios::ate);
	if (!readFile)
	{
		std::cout << "Nie udalo sie otworzyz pliku o nazwie: " << filename << std::endl;
		readFile.close();
		return Result::Fail;
	}
	std::cout << "Wysylam uzytkownikowi <" << name << "> plik " << filename << std::endl;
	size_t fileSize = readFile.tellg();
	readFile.seekg(0);
	char buffer[1025] = {};
	int bytesLeft = 0;
	

	Message prepareFile;
	std::string fileCmd =  prepareFile.PrepareCommand(filename, ownLogin, Command::FileName);

	if (user != connections.end())
	{
		user->connection.SendAll(fileCmd.c_str(), fileCmd.size());
	}
	else
	{
		auto userNotConn = std::find_if(availableClients.begin(), availableClients.end(), [name](clientInfo client) {return client.name == name; });
		if (userNotConn != availableClients.end())
		{
			Socket newClientSocket(Protocol::TCP);
			if (newClientSocket.Create() == Result::Success)
			{
				if (newClientSocket.Connect(userNotConn->listening) == Result::Success)
				{
					userNotConn->connection = newClientSocket;

					connections.push_back(*userNotConn);
					FD_SET(newClientSocket.GetSocket(), &master_fd);

					Message login;
					std::string msgLogin = login.PrepareCommand("", ownLogin, Command::LogIn);
					if (userNotConn->connection.SendAll(msgLogin.c_str(), msgLogin.size()) != Result::Success)
					{
						// cos poszlo nie tak
						return Result::Fail;
					}
					else
					{
						userNotConn->connection.SendAll(fileCmd.c_str(), fileCmd.size());

					}
				}
			}
		}
	}
	int bytes_sent = 0;
	auto userToSend = std::find_if(connections.begin(), connections.end(), [name](clientInfo client) {return client.name == name; });
	while (bytes_sent < fileSize)
	{
		
		readFile.read(buffer, 1024);
		auto readBytes = readFile.gcount();
		bytes_sent += readBytes;
		std::string buf(buffer, readBytes);
		Message fileToSend;
		std::string msgToSend = fileToSend.PrepareFile(buf, ownLogin, fileSize);
		userToSend->connection.SendAll(msgToSend.c_str(), msgToSend.size());
		//auto bytesSent = readFile.gcount();

		//auto actualPos = readFile.tellg();
		bytesLeft = fileSize - bytes_sent;
		//bytesLeft = fileSize - actualPos;
		float percent = (float)bytes_sent / fileSize;

		int barWidth = 60;
		std::cout << "[";
		int pos = barWidth * percent;
		for (int i = 0; i < barWidth; ++i) {
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		std::cout << "] " << int(percent * 100.0) << " " << std::setw(3) << "% " << bytes_sent << '\r';
		std::cout.flush();
	}
	std::cout << std::endl;
	readFile.close();
	
	return Result::Success;
}

Result Client::Input(std::string input)
{
	std::string name, fileName, msg;
	if (input.at(0) == '@')
	{
		std::stringstream exp(input);
		exp.seekg(1);
		exp >> name;
		if (name.find('#') != std::string::npos)
		{
			std::replace(name.begin(), name.end(), '#', ' ');
			std::stringstream expcmd(name);
			expcmd >> name >> fileName;
			if (fileName != "")
			{
				//cos zrob
				SendFile(name, fileName);
				std::cout << fileName << " zostal wyslany." << std::endl;
				return Result::Success;
			}
			else
			{
				expcmd.seekg(1, std::ios_base::cur);
				std::getline(expcmd, msg);
			}
		}
		else
		{
			exp.seekg(1, std::ios_base::cur);
			std::getline(exp, msg);
		}
	}
	else
	{
		name = "server";
		msg = input;
	}

	SendMsg(name, msg);

	return Result::Success;
}

//(VC++ Directories) Add Include Dir -> Solution Dir
//(VC++ Directories) Add Library Dir -> Output Dir
//(Linket/Input) Add Additional Dep -> SocketLibrary.lib
//(Solution/Project Build Order/Dependecies) Correct Oreder Build
//(Linker/Input) Add Additional Dep -> ws2_32.lib