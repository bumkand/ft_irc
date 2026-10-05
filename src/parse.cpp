#include "Ircserv.hpp"
#include "ClientData.hpp"
#include "Message.hpp"
#include "cmdType.hpp"
#include "Replies.hpp"

static void parseParams(Message& m, const std::string& completeMessage, size_t start, size_t end)
{
	end = completeMessage.find(' ', start);
	while (end != std::string::npos && start < completeMessage.length() && completeMessage[start] != ':')
	{
		m.pushBackParam(completeMessage.substr(start, end - start));
		start = completeMessage.find_first_not_of(' ', end);
		end = completeMessage.find(' ', start);
	}
	if (start < completeMessage.length() && completeMessage[start] == ':')
	{
		m.pushBackParam(completeMessage.substr(start + 1)); //can result in an empty parameter athe the end of vector if the command ends with ":"
	}
	else if (end == std::string::npos && start < completeMessage.length())
	{
		m.pushBackParam(completeMessage.substr(start));
	}
}

static Message parseMessage(const std::string& completeMessage)
{
	std::cout << "complete Message: " << completeMessage << std::endl;
	Message m;
	size_t start = 0;
	size_t end = 0;

	if (completeMessage.empty())
		return m;
	if (completeMessage[0] == ':')
	{
		end = completeMessage.find(' ');
		if (end != std::string::npos)
		{
			m.setPrefix(completeMessage.substr(1, end - 1));
			start = completeMessage.find_first_not_of(' ', end);
		}
	}
	end = completeMessage.find(' ', start);
	if (end != std::string::npos)
	{
		m.setCommand(my_toupper(completeMessage.substr(start, end - start)));
		start = completeMessage.find_first_not_of(' ', end);
	}
	else if (start < completeMessage.length())
	{
		m.setCommand(my_toupper(completeMessage.substr(start)));
		return m;
	}
	else
		return m;
	parseParams(m, completeMessage, start, end);
	return m;
}

void Ircserv::handleMessage(const Message& m, size_t i)
{
	std::string cmds[16] = {"PASS", "NICK", "USER", "CAP", "PING", "PONG", "QUIT", "NOTICE", "PRIVMSG", "JOIN", "PART", "KICK", "INVITE", "TOPIC", "MODE", "WHO"};
	int j;
	for (j = 0; j < 16; j++) {
		if (m.getCommand() == cmds[j]) {
			break;
		}
	}
	switch (j) {
		case CMD_PASS:
			handlePass(m, i);
			break;
		case CMD_NICK:
			handleNick(m, i);
			break;
		case CMD_USER:
			handleUser(m, i);
			break;
		case CMD_CAP:
			handleCap(m, i);
			break;
		case CMD_PING:
			handlePing(m, i);
			break;
		case CMD_PONG:
			break;
		case CMD_QUIT:
			handleQuit(m, i);
			break;
		case CMD_NOTICE:
			handlePrivMsg(m, i, true);
			break;
		case CMD_PRIVMSG:
			handlePrivMsg(m, i, false);
			break;
		case CMD_JOIN:
			if (!_data[i].isRegistered())
				_data[i].sendMsg(ERR_NOTREGISTERED(_data[i].getNick()));
			else
				_channels.join(&_data[i], m.getParams());
			break;
		case CMD_PART:
			if (!_data[i].isRegistered())
				_data[i].sendMsg(ERR_NOTREGISTERED(_data[i].getNick()));
			else
				_channels.part(&_data[i], m.getParams());
			break;
		case CMD_KICK:
			if (!_data[i].isRegistered())
				_data[i].sendMsg(ERR_NOTREGISTERED(_data[i].getNick()));
			else
				_channels.kick(&_data[i], m.getParams());
			break;
		case CMD_INVITE:
			if (!_data[i].isRegistered())
				_data[i].sendMsg(ERR_NOTREGISTERED(_data[i].getNick()));
			else
				_channels.invite(&_data[i], m.getParams());
			break;
		case CMD_TOPIC:
			if (!_data[i].isRegistered())
				_data[i].sendMsg(ERR_NOTREGISTERED(_data[i].getNick()));
			else
				_channels.topic(&_data[i], m.getParams());
			break;
		case CMD_MODE:
			if (!_data[i].isRegistered())
				_data[i].sendMsg(ERR_NOTREGISTERED(_data[i].getNick()));
			else
				_channels.mode(&_data[i], m.getParams());
			break;
		case CMD_WHO:
			if (!_data[i].isRegistered())
				_data[i].sendMsg(ERR_NOTREGISTERED(_data[i].getNick()));
			else
				_channels.who(&_data[i], m.getParams());
			break;
		default:
			_data[i].sendMsg(ERR_UNKNOWNCOMMAND(_data[i].getNick(), m.getCommand()));
			std::cerr << "invalid cmd" << std::endl;
	}
}

void Ircserv::parse(size_t i)
{
	size_t pos = _data[i].getStr().find("\r\n");

	while (pos != std::string::npos) 
	{
		std::string completeMessage = _data[i].getStr().substr(0, pos);
		_data[i].eraseStr(0, pos + 2);
		Message m = parseMessage(completeMessage);
		if (!m.getPrefix().empty() && my_tolower(m.getPrefix()) != my_tolower(_data[i].getNick()))
		{
			pos = _data[i].getStr().find("\r\n");
			continue;
		}
		handleMessage(m, i);
		pos = _data[i].getStr().find("\r\n");
	}
}
