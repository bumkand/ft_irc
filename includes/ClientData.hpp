#ifndef CLIENTDATA_HPP
#define CLIENTDATA_HPP

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <exception>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/socket.h>

class	ClientData
{
	private:
		int			_fd;
		std::string	_str;
		bool		_auth;
		std::string _username;
		std::string _nick;

	public:
		ClientData();
		ClientData(const ClientData& other);
		ClientData& operator=(const ClientData& other);
		virtual ~ClientData();

		void		setFD(const int fd);
		int			getFD(void) const;
		void		setStr(const std::string str);
		std::string	getStr(void) const;
		void		clearStr(void);
		void		appendStr(char* buffer, ssize_t recBite);

};



#endif