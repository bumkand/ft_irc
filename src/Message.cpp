#include "Message.hpp"

Message::Message()
{
}

Message::Message(const Message &copy)
{
	_prefix = copy._prefix;
	_command = copy._command;
	_params = copy._params;
}


Message::~Message()
{
}


Message & Message::operator=(const Message &assign)
{
	if (this != &assign)
	{
		_prefix = assign._prefix;
		_command = assign._command;
		_params = assign._params;
	}
	return *this;
}

const std::string& Message::getPrefix() const
{
	return _prefix;
}

const std::string& Message::getCommand() const
{
	return _command;
}

const std::string& Message::getParam(size_t i) const
{
	if (i < _params.size())
	{
		return _params[i];
	}
	static const std::string empty = "";
	return empty;
}

size_t		Message::getParamSize() const
{
	return _params.size();
}

void Message::setPrefix(const std::string &prefix)
{
	_prefix = prefix;
}

void Message::setCommand(const std::string &command)
{
	_command = command;
}

void Message::pushBackParam(const std::string &param)
{
	_params.push_back(param);
}
