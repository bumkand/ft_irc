#include "ClientData.hpp"

ClientData::ClientData(): _fd(-1), _registered(0)
{
	//std::cout << "Default constructor called" << std::endl;
}

ClientData::ClientData(const ClientData& other) :
	_fd(other._fd), _str(other._str), _registered(other._registered), _nick(other._nick), _username(other._username)
{
	//std::cout << "Copy constructor called" << std::endl;
}

ClientData& ClientData::operator=(const ClientData& other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_str = other._str;
		_registered = other._registered;
		_nick = other._nick;
		_username = other._username;
	}
	//std::cout << "Assigment operator called" << std::endl;
	return *this;
}

ClientData::~ClientData()
{
	//std::cout << "Destructor called" << std::endl;
}

const std::string &ClientData::getNick() const
{
	return _nick;
}

const std::string &ClientData::getUser() const
{
	return _username;
}

bool ClientData::isRegistered() const
{
	return _registered;
}

void		ClientData::setFD(const int fd)
{
	_fd = fd;
}

int			ClientData::getFD(void) const
{
	return _fd;
}

void		ClientData::setStr(const std::string str)
{
	_str = str;
}

std::string	ClientData::getStr(void) const
{
	return _str;
}

void		ClientData::clearStr(void)
{
	_str.clear();
}

void		ClientData::appendStr(char* buffer, ssize_t recBite)
{
	_str.append(buffer, recBite);
}

void		ClientData::eraseStr(size_t pos, size_t len)
{
	_str.erase(pos, len);
}

void ClientData::sendMsg(const std::string &msg)
{
	_outBuf += msg;
}
