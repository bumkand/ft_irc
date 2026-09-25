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
		size_t			_activeClients;
		ClientData	_data[MAX_CLIENTS];
		
		void	handleMessage(const Message &m, size_t i);
		void	handlePass(const Message &m, size_t i);
		bool	checkNickFree(const Message &m);
		void	handleNick(const Message &m, size_t i);
		// void	handleUser(const Message &m, size_t i);
		void	handleCap(const Message &m, size_t i);
		// void	handlePing(const Message &m, size_t i);
		// void	handlePong(const Message &m, size_t i);
		// void	handleQuit(const Message &m, size_t i);
		// void	handleNotice(const Message &m, size_t i);
		// void	handlePrivMsg(const Message &m, size_t i);

		void 	parse(size_t i);

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
};

	void	signalHandler(int sig);

#endif