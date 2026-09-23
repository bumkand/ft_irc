#include "Ircserv.hpp"
#include "ClientData.hpp"
#include "Message.hpp"
#include "Replies.hpp"

void	Ircserv::handlePass(Message m, size_t i)
{
	if (_data[i].isRegistered() && _data[i].getNick().empty())
	{
		_data[i].sendMsg(ERR_ALREADYREGISTRED("*"));
	}
	else if (_data[i].isRegistered())
	{
		_data[i].sendMsg(ERR_ALREADYREGISTRED(_data[i].getNick()));
	}
	else if (m.getParamSize() < 1 || m.getParam(0).empty())
	{
		_data[i].sendMsg(ERR_NEEDMOREPARAMS("*", "PASS"));
	}
	else if (m.getParam(0) != _password)
	{
		_data[i].sendMsg(ERR_PASSWDMISMATCH("*"));
	}
	else
	{
		_data[i].setPassed(true);
		std::cout << "client has passed" << std::endl;
	}
}

// void	Ircserv::handleNick(Message m, size_t i);
// void	Ircserv::handleUser(Message m, size_t i);

void	Ircserv::handleCap(Message m, size_t i)
{
	if (m.getCommand() == "CAP")
	{
		_data[i].sendMsg("CAP * LS :\r\n");
		std::cout << "capabilities established" << std::endl;
	}
}

