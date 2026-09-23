#ifndef IRCSERV_HPP
#define IRCSERV_HPP

#include <cstring>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <exception>
#include <csignal>
#include <unistd.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "ClientData.hpp"
#include "Message.hpp"

#define MAX_CLIENTS 100


class	Ircserv
{
	private:
		int			_serverSocket;
		int			_port;
		sockaddr_in	_serverAddress;
		std::string	_password;
		int			_clientSocket;
		pollfd		_pfds[MAX_CLIENTS];
		int			_activeClients;
		ClientData	_data[MAX_CLIENTS];
		
		void	handleMessage(Message m, size_t i);
		void	handlePass(m, i);
		// void	handleNick(m, i);
		// void	handleUser(m, i);
		void	handleCap(m, i);
		// void	handlePing(m, i);
		// void	handlePong(m, i);
		// void	handleQuit(m, i);
		// void	handleNotice(m, i);
		// void	handlePrivMsg(m, i);

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

		void	closeSocket();
		void 	parse(size_t i);
};

	void	signalHandler(int sig);

#endif