#include "Ircserv.hpp"
#include "ClientData.hpp"

void Ircserv::parseCommand(std::string completeCommand)
{
	// std::cout << "complete command: " << completeCommand << std::endl;
	std::string prefix;
	if (completeCommand[0] == ':')
	{
		size_t len = completeCommand.find(' ');
		if (len != std::string::npos)
			prefix = completeCommand.substr(1, len - 1);
		// else
		// {

		// }
	}

}

void Ircserv::parse(int i)
{
	size_t pos = _data[i].getStr().find("\r\n");

	while (pos != std::string::npos) 
	{
		std::string completeCommand = _data[i].getStr().substr(0, pos);
		_data[i].setStr(_data[i].getStr().substr(pos + 2));
		parseCommand(completeCommand);
		pos = _data[i].getStr().find("\r\n");
	}
}
