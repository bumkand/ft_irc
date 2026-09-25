#include "Ircserv.hpp"
#include "ClientData.hpp"
#include "Message.hpp"
#include "Replies.hpp"

void	Ircserv::welcomeSequence(size_t i)
{
	_data[i].sendMsg(RPL_WELCOME(_data[i].getNick(), _data[i].getFullMask()));
	_data[i].sendMsg(RPL_YOURHOST(_data[i].getNick()));
	_data[i].sendMsg(RPL_CREATED(_data[i].getNick()));
	_data[i].sendMsg(RPL_MYINFO(_data[i].getNick()));
	std::cout << "welcome sequence sent" << std::endl;//remove
}

void	Ircserv::handlePass(const Message &m, size_t i)
{
	if (_data[i].isRegistered())
	{
		_data[i].sendMsg(ERR_ALREADYREGISTRED(_data[i].getNick()));
	}
	else if (m.getParamSize() < 1 || m.getParam(0).empty())
	{
		_data[i].sendMsg(ERR_NEEDMOREPARAMS(_data[i].getNick(), "PASS"));
	}
	else if (m.getParam(0) != _password)
	{
		_data[i].sendMsg(ERR_PASSWDMISMATCH(_data[i].getNick()));
		std::cout << "wrong server password" << std::endl;//remove
	}
	else
	{
		_data[i].setPassed(true);
		std::cout << "client has passed" << std::endl;//remove
	}
}

void	Ircserv::handleUser(const Message &m, size_t i)
{
	if (_data[i].isRegistered()) {
		_data[i].sendMsg(ERR_ALREADYREGISTRED(_data[i].getNick()));
	}
	else if (m.getParamSize() < 4) {
		_data[i].sendMsg(ERR_NEEDMOREPARAMS(_data[i].getNick(), "USER"));
	}
	else if (!(_data[i].getNick() == "*") && !(_data[i].hasPassed())) {
		_data[i].sendMsg(ERR_PASSWDMISMATCH(_data[i].getNick()));
	}
	else {
		_data[i].setUsername(m.getParam(0));
		_data[i].setRealname(m.getParam(3));
		if (_data[i].hasPassed() && _data[i].getNick() != "*")
		{
			_data[i].setRegistered(true);
			welcomeSequence(i);
		}
	}
}

void	Ircserv::handleCap(const Message &m, size_t i)
{
	if (m.getParamSize() > 0 && m.getParam(0) == "LS")
	{
		_data[i].sendMsg("CAP * LS :\r\n");
		std::cout << "capabilities established" << std::endl;//remove
	}
}

