#include "Ircserv.hpp"
#include "ClientData.hpp"
#include "Message.hpp"
#include "cmdType.hpp"

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
		m.setCommand(completeMessage.substr(start, end - start));
		start = completeMessage.find_first_not_of(' ', end);
	}
	else if (start < completeMessage.length())
	{
		m.setCommand(completeMessage.substr(start));
		return m;
	}
	else
		return m;
	parseParams(m, completeMessage, start, end);
	return m;
}

void Ircserv::handleMessage(const Message& m, size_t i)
{
	std::string cmds[15] = {"PASS", "NICK", "USER", "CAP", "PING", "PONG", "QUIT", "NOTICE", "PRIVMSG", "JOIN", "PART", "KICK", "INVITE", "TOPIC", "MODE"};
	int j;
	for (j = 0; j < 15; j++) {
		if (m.getCommand() == cmds[j]) {
			break;
		}
	}
	switch (j) {
		case CMD_PASS:
			handlePass(m, i);
			break;
		case CMD_NICK:
			// handleNick(m, i);
			break;
		case CMD_USER:
			// handleUser(m, i);
			break;
		case CMD_CAP:
			handleCap(m, i);
			break;
		case CMD_PING:
			// handlePing(m, i);
			break;
		case CMD_PONG:
			// handlePong(m, i);
			break;
		case CMD_QUIT:
			// handleQuit(m, i);
			exit(0);
			break;
		case CMD_NOTICE:
			// handleNotice(m, i);
			break;
		case CMD_PRIVMSG:
			std::cout << "privmsg\n   to:" << m.getParam(0) << "\n   content:" << m.getParam(1) << std::endl;
			// handlePrivMsg(m, i);
			break;
		case CMD_JOIN:
			break;
		case CMD_PART:
			break;
		case CMD_KICK:
			break;
		case CMD_INVITE:
			break;
		case CMD_TOPIC:
			break;
		case CMD_MODE:
			break;
		default:
			_data[i].sendMsg("cmd not found");
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
		//check if a message is valid (prefix is trully the nick of the client it came from, command is all letters (and not empty), params are correct)

		// std::cout << "prefix: " << m.getPrefix() << std::endl;
		// std::cout << "command: " << m.getCommand() << std::endl;
		// std::cout << "params: ";
		// for (size_t j = 0; j < m.getParamSize(); ++j) 
		// {
		// 	// Wrapping the string in brackets makes it easy to spot trailing spaces or empty strings
		// 	std::cout << "[" << m.getParam(j) << "] "; 
		// }
		// std::cout << std::endl;

		handleMessage(m, i);
		pos = _data[i].getStr().find("\r\n");
	}
}
