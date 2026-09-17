#ifndef IRCSERV_HPP
#define IRCSERV_HPP

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_CLIENTS 100


struct	clientData
{
	int				fd;
	std::string		str;
};

class	Ircserv
{
	private:
		int			_serverSocket;
		int			_port;
		sockaddr_in	_serverAddress;
		int			_clientSocket;
		pollfd		_pfds[MAX_CLIENTS];
		int			_activeClients;

		clientData	_data[MAX_CLIENTS];

		int			end;
		

	public:
		Ircserv();
		Ircserv(const Ircserv& other);
		Ircserv& operator=(const Ircserv& other);
		~Ircserv();
		void	initServ(char *arv[]);
		void	servLoop();
		void	addNewClient();
		void	existClient();



};

#endif