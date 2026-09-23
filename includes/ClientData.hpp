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
		bool		_hasPassed;
		bool		_registered;
		std::string _nick;
		std::string _username; //will be changed
		std::string _outBuf;

	public:
		ClientData();
		ClientData(const ClientData& other);
		ClientData& operator=(const ClientData& other);
		virtual ~ClientData();

		int					getFD(void) const;
		std::string			getStr(void) const;
		const std::string	&getNick() const;
		const std::string 	&getUser() const;
		bool 				isRegistered() const;
		bool 				hasPassed() const;

		void				setFD(const int fd);
		void				setStr(const std::string &str);
		void				setNick(const std::string &nick);
		void				setUser(const std::string &user);
		void				setRegistred(bool registred);
		void				setPassed(bool passed);

		void				clearStr(void);
		void				appendStr(char* buffer, ssize_t recBite);
		void				eraseStr(size_t pos, size_t len);

		void				clearOutBuf(void);
		void 				sendMsg(const std::string &msg);
};



#endif