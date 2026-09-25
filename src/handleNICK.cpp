#include "Ircserv.hpp"
#include "ClientData.hpp"
#include "Message.hpp"
#include "Replies.hpp"

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

static std::string my_tolower(const std::string &str)
{
	std::string result;
	result.resize(str.length());

	for (size_t j = 0; j < str.length(); j++)
	{
		if (str[j] >= 'A'&& str[j] <= '^')
		{
			result[j] = str[j] + ('a' - 'A');
		}
		else
		{
			result[j] = str[j];
		}
	}
	return result;
}

bool Ircserv::checkNickFree(const Message &m)
{
	for (size_t j = 0; j < _activeClients; j++)
	{
		if (m.getParam(0).length() != _data[j].getNick().length())
		{
			continue;
		}
		if (my_tolower(m.getParam(0)) == my_tolower(_data[j].getNick()))
		{
			return false;
		}
	}
	return true;
}

void	Ircserv::handleNick(const Message &m, size_t i)
{
	if (!(_data[i].getUsername().empty()) && !(_data[i].hasPassed())) {
		_data[i].sendMsg(ERR_PASSWDMISMATCH(_data[i].getNick()));
	}
	else if (m.getParamSize() < 1 || m.getParam(0).empty()) {
		_data[i].sendMsg(ERR_NONICKNAMEGIVEN(_data[i].getNick()));
	}
	else if (my_tolower(_data[i].getNick()) == my_tolower(m.getParam(0))) {
		_data[i].setNick(m.getParam(0));
	}
	else if (!checkNickValidity(m)) {
		_data[i].sendMsg(ERR_ERRONEUSNICKNAME(_data[i].getNick(), m.getParam(0)));
	}
	else if (!checkNickFree(m)) {
		_data[i].sendMsg(ERR_NICKNAMEINUSE(_data[i].getNick(), m.getParam(0)));
	}
	else if (!_data[i].isRegistered()) {
		_data[i].setNick(m.getParam(0));
		std::cout << "new client set their nick to: " << _data[i].getNick() << std::endl;//remove
		if (_data[i].hasPassed() && !(_data[i].getUsername().empty()))
		{
			_data[i].setRegistered(true);
			welcomeSequence(i);
		}
	}
	else {
		std::string oldFullMask = _data[i].getFullMask();
		_data[i].setNick(m.getParam(0));
		_data[i].sendMsg(":" + oldFullMask + " NICK :" + _data[i].getNick() + "\r\n");
		// send ":oldnick!user@host NICK :newnick" to everyone who shares at least one channel with this client
	}
}
