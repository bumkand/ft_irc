#include "Ircserv.hpp"
#include "ClientData.hpp"
#include "Message.hpp"
#include "Replies.hpp"

void	Ircserv::handlePass(m, i)
{
	if (_data[i].isRegistered() && _data[i].getNick().empty())
	{
		_data[i].sendMsg(ERR_ALREADYREGISTRED("*", "PASS"));
	}
	else if (_data[i].isRegistered())
	{
		_data[i].sendMsg(ERR_ALREADYREGISTRED(_data[i].getNick(), "PASS"));
	}
	else if (m.getParamSize() < 1 || m.getParam(0).empty())
	{
		_data[i].sendMsg(ERR_NEEDMOREPARAMS("*", "PASS"));
	}
	else if (m.getParam(0) != _password)
	{
		_data[i].sendMsg(ERR_PASSWDMISMATCH("*", "PASS"));
	}
	else
	{
		_data[i].setPassed(true);
	}
}

// void	Ircserv::handleNick(m, i);
// void	Ircserv::handleUser(m, i);

void	Ircserv::handleCap(m, i)
{
	_data[i].sendMsg("CAP * LS :\r\n");
}

