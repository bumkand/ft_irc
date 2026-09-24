#include "Ircserv.hpp"
#include "ClientData.hpp"
#include "Message.hpp"
#include "Replies.hpp"

void	Ircserv::handlePass(const Message &m, size_t i)
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
		std::cout << "client has passed" << std::endl;//remove
	}
}

static bool checkNickValidity(const Message &m)
{
	std::string nick = m.getParam(0);
	if (nick.length() > 9)
	{
		return false;
	}
	if (!(nick[0] >= 'A' && nick[0] <= '}'))
	{
		return false;
	}
	for (size_t j = 1; j < nick.length(); j++)
	{
		if (!((nick[j] >= 'A' && nick[j] <= '}') || (nick[j] >= '0' && nick[j] <= '9') || nick[j] == '-'))
		{
			return false;
		}
	}
	return true;
}

bool Ircserv::checkNickFree(const Message &m, size_t i)
{
	for (size_t j = 0; j < _activeClients; j++)
	{
		if (i != j && m.getParam(0) == _data[i].getNick())
		{
			return false;
		}
	}
	return true;
}

void	Ircserv::handleNick(const Message &m, size_t i)
{
	std::string oldNick = "*";
	if (!(_data[i].getNick().empty())) {
		oldNick = _data[i].getNick();
	}

	if (!(_data[i].getUser().empty()) && !(_data[i].hasPassed())) {
		_data[i].sendMsg(ERR_NOTREGISTERED(oldNick));
	}
	else if (m.getParamSize() < 1 || m.getParam(0).empty()) {
		_data[i].sendMsg(ERR_NONICKNAMEGIVEN(oldNick));
	}
	else if (_data[i].getNick() == m.getParam(0)) {
		return;
	}
	else if (!checkNickValidity(m)) {
		_data[i].sendMsg(ERR_ERRONEUSNICKNAME(oldNick, m.getParam(0)));
	}
	else if (!checkNickFree(m, i)) {
		_data[i].sendMsg(ERR_NICKNAMEINUSE(oldNick, m.getParam(0)));
	}
	else if (!_data[i].isRegistered()) {
		_data[i].setNick(m.getParam(0));
		if (_data[i].hasPassed() && !(_data[i].getUser().empty()))
		{
			_data[i].setRegistered(true);
			// start the welcome sequence 001, 002, 003
		}
	}
	else {
		_data[i].setNick(m.getParam(0));
		// change nick everywhere and broadcast to people that the nick has changed
	}
}

// void	Ircserv::handleUser(const Message &m, size_t i);

void	Ircserv::handleCap(const Message &m, size_t i)
{
	if (m.getCommand() == "CAP")
	{
		_data[i].sendMsg("CAP * LS :\r\n");
		std::cout << "capabilities established" << std::endl;
	}
}

