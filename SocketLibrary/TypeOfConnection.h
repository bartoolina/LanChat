#pragma once

namespace SocketLibrary
{
	enum class TypeOfConnection
	{
		None,
		Listener,
		Reciever,
		Sender,
		TwoWay,
	};
}