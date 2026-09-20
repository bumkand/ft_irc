#ifndef IRCSERV_HPP
#define IRCSERV_HPP

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <exception>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "ClientData.hpp"

#define MAX_CLIENTS 100


class	Ircserv : public ClientData
{
	private:
		int			_serverSocket;
		int			_port;
		sockaddr_in	_serverAddress;
		int			_clientSocket;
		pollfd		_pfds[MAX_CLIENTS];
		int			_activeClients;
		ClientData	_data[MAX_CLIENTS];
		int			end;
		

	public:
		Ircserv();
		Ircserv(const Ircserv& other);
		Ircserv& operator=(const Ircserv& other);
		virtual ~Ircserv();
		void	initServ(char *arv[]);
		void	servLoop();
		void	addNewClient();
		void	existClient();

		class ErrorException : public std::exception
		{
			const char* what() const throw();
		};


};

#endif