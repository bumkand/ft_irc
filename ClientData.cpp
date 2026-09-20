#include "includes/ClientData.hpp"

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
