#include "Ircserv.hpp"
#include "ClientData.hpp"
#include "Message.hpp"
#include "Replies.hpp"

void Ircserv::nickPrivMsg(const std::string &receiver, const std::string &msg)
{
	for (size_t j = 0; j < MAX_CLIENTS; j++)
	{
		if (_data[j].getFD() != -1 && my_tolower(receiver) == my_tolower(_data[j].getNick()))
		{
			_data[j].sendMsg(msg);
			std::cout << "privmsg got send" << std::endl;
			return;
		}
	}
}

void Ircserv::handlePrivMsg(const Message &m, size_t i, bool notice)
{
	if (!_data[i].isRegistered())
	{
		_data[i].sendMsg(ERR_NOTREGISTERED(_data[i].getNick()));
	}
	else if (m.getParamSize() == 0)
	{
		if (!notice)
		{
			_data[i].sendMsg(ERR_NORECIPIENT(_data[i].getNick(), m.getCommand()));
		}
	}
	else if (m.getParamSize() == 1 || m.getParam(1).empty())
	{
		if (!notice)
		{
			_data[i].sendMsg(ERR_NOTEXTTOSEND(_data[i].getNick()));
		}
	}
	else
	{
		std::string text = m.getParam(1);
		for (size_t j = 2; j < m.getParamSize(); j++)
		{
			if (!m.getParam().empty()) {
				text += " " + m.getParam(j);
			}
		}
		std::vector<std::string> receivers = split(m.getParam(0), ',');
		for (size_t j = 0; j < receivers.size(); j++)
		{
			if (receivers[j][0] == '#' || receivers[j][0] == '&')
			{
				_channels.privmsg(&_data[i], receivers[j], text, notice);
			}
			else
			{
				nickPrivMsg(receivers[j], ":" + _data[i].getFullMask() + " " + m.getCommand() + " " + receivers[j] + " :" + text + "\r\n");
			}
		}
	}
}
