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
	std::cout << "welcome sequence sent" << std::endl;
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
		std::cout << "wrong server password" << std::endl;
	}
	else
	{
		_data[i].setPassed(true);
		std::cout << "client has passed" << std::endl;
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
		_data[i].sendMsg(":" SERVER_NAME " CAP * LS :\r\n");
		std::cout << "capabilities established" << std::endl;
	}
}

void	Ircserv::handlePing(const Message &m, size_t i)
{
	std::string parameter = "";
	if (m.getParamSize() > 0)
	{
		parameter = m.getParam(0);
	}
	_data[i].sendMsg(":" SERVER_NAME " PONG " SERVER_NAME " :" + parameter + "\r\n");
}

void	Ircserv::handleQuit(const Message &m, size_t i)
{
	std::string quitMgs = _data[i].getNick();
	if (m.getParamSize() > 0)
	{
		quitMgs = m.getParam(0);
	}
	_channels.removeClient(&_data[i], quitMgs); // tell his channels + take him out
	_data[i].resetClient();
	close(_pfds[i].fd);
	_pfds[i].fd = -1;
	_activeClients--;
}