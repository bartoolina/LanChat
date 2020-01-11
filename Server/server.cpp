//Server
//#include <SocketLibrary/SocketLibrary.h>
#include <iostream>
#include <algorithm>
#include <iomanip>

#include "Server.h"


//(VC++ Directories) Add Include Dir -> Solution Dir
//(VC++ Directories) Add Library Dir -> Output Dir
//(Linket/Input) Add Additional Dep -> SocketLibrary.lib
//(Solution/Project Build Order/Dependecies) Correct Oreder Build
//(Linker/Input) Add Additional Dep -> ws2_32.lib

Result Server::Initialize(Endpoint ip)
{
	connections.clear();
	FD_ZERO(&master_fd);
	ownEndpoint = ip;

	if (Network::Initialize())
	{
		// Winsock dziala
		std::cout << "[SERVER] Winsock zostal poprawnie zainicjalizowany." << std::endl;

		listeningSocketTCP = Socket(Protocol::TCP);
		if (listeningSocketTCP.Create() == Result::Success)
		{
			//  Socket listening zostal utworzony
			std::cout << "[SERVER] Gniazdo TCP do nasluchu zostalo utworzone." << std::endl;

			if (listeningSocketTCP.BindAndListen(ownEndpoint) == Result::Success)
			{
				// Socket listening od teraz nasluchuje
				std::cout << "[SERVER] Gniazdo TCP do nasluchu nasluchuje." << std::endl;
				// Dodanie socketa do listy 
				FD_SET(listeningSocketTCP.GetSocket(), &master_fd);

			}
			else
			{
				std::cerr << "[SERVER] Nie udalo sie wlaczyc nasluchu na gniezdie TCP." << std::endl;
				listeningSocketTCP.Close();
				return Result::Fail;
			}
		}
		else
		{
			std::cerr << "[SERVER] Nie mozna utworzych gniazda TCP do nasluchu." << std::endl;
			return Result::Fail;
		}
				
		broadcastSenderUDP = Socket(Protocol::UDP_Sender);
		if (broadcastSenderUDP.Create() == Result::Success)
		{
			//  Socket  zostal utworzony
			std::cout << "[SERVER] Gniazdo UDP do informowania zostalo utworzone." << std::endl;

			// TODO - udp
			if (broadcastSenderUDP.AssignEndpoint(Endpoint("255.255.255.255", 54000)) == Result::Fail)
			{
				// error
			}
			
		}
		else
		{
			std::cerr << "[SERVER] Nie mozna utworzych gniazda UDP do broadcastu." << std::endl;
			return Result::Fail;
		}
	}
	return Result::Success;
}

Result Server::IncomingConnection()
{
	//Sleep(1000);

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
				if (listeningSocketTCP.Accept(newConnectionSocket) == Result::Success )
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
			
			else if (copy_fd.fd_array[i] == broadcastSenderUDP.GetSocket())
			{
				// TODO
				//continue;
			}
			
			else
			{
				Message recived;
				std::string recivedMessage;
				int bytesRecived;
				
				SOCKET lookingFor = copy_fd.fd_array[i];
				auto findedClient =  std::find_if(connections.begin(), connections.end(), [lookingFor](clientInfo clientInfo) { return lookingFor == clientInfo.connection.GetSocket(); });
				
				switch (findedClient->connection.ReciveAll(recivedMessage, bytesRecived))
				{
				case Result::Fail:
				case Result::ConnectionLost:
				{
					std::string nameLogOut = findedClient->name;
					FD_CLR(findedClient->connection.GetSocket(), &master_fd);
					findedClient->connection.Close();
					connections.erase(findedClient);

					for (auto Client : connections)
					{
						if (!Client.name.empty())
						{
							// wysyla do pozostalych zalogowanych uzytkowniko kto sie wylogowal
							//if (Client.connection.GetSocket() != findedClient->connection.GetSocket())
							//{
								Message clientLogOut;

								std::string msg = clientLogOut.PrepareCommand("", nameLogOut, Command::LogOut);
								if (Client.connection.SendAll(msg.c_str(), msg.size()) != Result::Success)
								{
									// cos poszlo nie tak
								}
							//}


						}
					}
				}
					break;

				case Result::Success:
					// TODO RecivesMessage BytesRecived
					findedClient->lastActiveTime = time(NULL);
					if (recived.Parse(recivedMessage.c_str(), bytesRecived) != Result::Success)
					{
						// wiadamosc niekompletna, bledna lub cos takiego
						return Result::Fail;
					}

					switch (recived.GetType())
					{
					case SocketLibrary::TypeOfMsg::Text:
						for (auto Client : connections)
						{
							if (Client.connection.GetSocket() != findedClient->connection.GetSocket())
							{
								if (Client.connection.SendAll(recivedMessage.c_str(), bytesRecived) != Result::Success)
								{
									// cos poszlo nie tak
								}
							}
						}
						break;
					case SocketLibrary::TypeOfMsg::File:
						break;
					case SocketLibrary::TypeOfMsg::Command:
					{
						switch (recived.GetCommand())
						{
						case Command::Unknown:
						{
							std::string clientIP = findedClient->connection.GetEndpoint().GetIpString();
							std::string clientPort = std::to_string( findedClient->connection.GetEndpoint().GetPort() );
							std::string clientSocket = std::to_string( findedClient->connection.GetSocket() );

							Log::PrintError("Nieprawidlowy lub brak komendy. [" + clientIP + ":" + clientPort + " SOCKET:" + clientSocket + "]", -1);
							return Result::Fail;
							break;
						}
						case Command::LogIn:
						{
							findedClient->name = recived.GetName();
							Log::Print(findedClient->name + " zalogowal sie z adresu" + findedClient->connection.GetEndpoint().GetIpString());
							for (auto Client : connections)
							{
								if (!Client.name.empty())
								{
									// wysyla do pozostalych zalogowanych uzytkownik, nowo zalogowanego uzytkownika
									// wysyla do nowo zalogowanego uzytkownika, pozostalych zalogowanych
									if (Client.connection.GetSocket() != findedClient->connection.GetSocket())
									{
										Message oldClients;
										std::string msgNewClientPort = std::to_string(findedClient->listening.GetPort());
										std::string msgToOtherClients = oldClients.PrepareCommand(msgNewClientPort,findedClient->name, Command::Adress);
										if (Client.connection.SendAll(msgToOtherClients.c_str(), msgToOtherClients.size()) != Result::Success)
										{
											// cos poszlo nie tak
											// pewnie zamknac polaczenie
										}

										Message newClient;
										std::string msgOldClientPort = std::to_string(Client.listening.GetPort());
										std::string msgToNewClients = newClient.PrepareCommand(msgOldClientPort ,Client.name, Command::Adress);
										if (findedClient->connection.SendAll(msgToNewClients.c_str(), msgToNewClients.size()) != Result::Success)
										{
											// cos poszlo nie tak
											// pewnie zamknac polaczenie
										}
									}


								}
							}
							break;
						}
						case Command::LogOut:
						{
							std::string nameLogOut = recived.GetName();
							findedClient->name = "";
							Log::Print(nameLogOut + " wylogowal sie z adresu" + findedClient->connection.GetEndpoint().GetIpString());
							for (auto Client : connections)
							{
								if (!Client.name.empty())
								{
									// wysyla do pozostalych zalogowanych uzytkowniko kto sie wylogowal
									if (Client.connection.GetSocket() != findedClient->connection.GetSocket())
									{
										Message clientLogOut;
										
										std::string msg = clientLogOut.PrepareCommand("", nameLogOut , Command::LogOut);
										if (Client.connection.SendAll(msg.c_str(), msg.size()) != Result::Success)
										{
											// cos poszlo nie tak
										}
									}
									

								}
							}
							break;
						}
						case Command::Adress:
						{
							int port = atoi(recived.GetMsg().c_str());
							Endpoint clientPortListenning = Endpoint(findedClient->connection.GetEndpoint().GetIpString().c_str(), (unsigned short)port);
							findedClient->listening = clientPortListenning;
							Log::Print("Odebralem port nasluchiwania klienta. [" + findedClient->listening.GetIpString() + ":" + std::to_string(findedClient->listening.GetPort()) + "]");

							//std::string newClientPort = std::to_string(findedClient->listening.GetPort());
							//for (auto Client : connections)
							//{
							//	
							//	// wysyla adresy (porty) do uzytkownikow
							//	if (Client.connection.GetSocket() != findedClient->connection.GetSocket())
							//	{
							//		Message oldClient;
							//		std::string msgToOtherClients = oldClient.PrepareCommand(newClientPort, "server", Command::Adress);
							//		if (Client.connection.SendAll(msgToOtherClients.c_str(), msgToOtherClients.size()) != Result::Success)
							//		{
							//			// cos poszlo nie tak
							//		}

							//		Message newClient;
							//		std::string oldClientPort = std::to_string(Client.listening.GetPort());
							//		std::string msgToNewClient = newClient.PrepareCommand(oldClientPort, "server", Command::Adress);
							//		if (findedClient->connection.SendAll(msgToNewClient.c_str(), msgToNewClient.size()) != Result::Success)
							//		{
							//			// cos poszlo nie tak
							//		}
							//	}
							//	
							//}
							break;
						}
						case Command::ConnCheck:
						{
							Log::Print("\"" + findedClient->name + "\" potwierdzil, ze jest wciaz aktywny.");
							break;						
						}
						default:
							break;
						}


						break;
					}
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

Result Server::CheckConnection()
{
	//Sleep(1000);
	if (sendUDP)
	{
		std::string msg, msg2;
		std::ostringstream msgSize;
		msg = "server:" + listeningSocketTCP.GetEndpoint().GetIpString() + ":" + std::to_string(listeningSocketTCP.GetEndpoint().GetPort());

		msgSize << std::setw(3) << std::setfill('0') << msg.size();
		msg = msgSize.str() + " " + msg;
		broadcastSenderUDP.SendUDP(msg, ownEndpoint);

	}

	/*for (auto client : connections)
	{
		Message chkMsg;
		std::string msg = chkMsg.PrepareCommand("", "server", Command::ConnCheck);
		if (client.lastActiveTime + 10 < time(NULL))
		{
			client.connection.SendAll(msg.c_str(), msg.size());

		}
	}*/

	return Result();
}



//for (auto sock = connections.begin(); sock != connections.end(); sock++)
//{
//
//	if (sock->GetSocket() == copy_fd.fd_array[i])
//	{
//		switch (sock->ReciveAll(recivedMessage, bytesRecived))
//		{
//		case Result::ConnectionLost:
//			FD_CLR(sock->GetSocket(), &master_fd);
//			connections.erase(sock);
//			sock->Close();
//			break;
//
//		case Result::Success:
//
//		case Result::Fail:
//
//		default:
//			break;
//		}
//	}
//}