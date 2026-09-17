#include "includes/Ircserv.hpp"
#include <signal.h>

Ircserv::Ircserv()
{
	//std::cout << "Default constructor called" << std::endl;
}

Ircserv::Ircserv(const Ircserv& other) :
	_serverSocket(other._serverSocket),
	_port(other._port),
	_serverAddress(other._serverAddress),
	_clientSocket(other._clientSocket),
	_activeClients(other._activeClients)
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
		_pfds[i] = other._pfds[i];
	for (int i = 0; i < MAX_CLIENTS; ++i)
		_data[i] = other._data[i];
	//std::cout << "Copy constructor called" << std::endl;
}

Ircserv& Ircserv::operator=(const Ircserv& other)
{
	if (this != &other)
	{
		_serverSocket = other._serverSocket;
		_port = other._port;
		_serverAddress = other._serverAddress;
		_clientSocket = other._clientSocket;
		_activeClients = other._activeClients;
		for (int i = 0; i < MAX_CLIENTS; i++)
			_pfds[i] = other._pfds[i];
		for (int i = 0; i < MAX_CLIENTS; i++)
			_data[i] = other._data[i];
	}
	//std::cout << "Assigment operator called" << std::endl;
	return *this;
}

Ircserv::~Ircserv()
{
	//std::cout << "Destructor called" << std::endl;
}

// Server socket initialization
void	Ircserv::initServ(char *arv[])
{
	std::stringstream	ss(arv[1]);
	ss >> _port;
	if (_port < 0 || _port > 65535)
	{
		std::cout << "Error: choose port between 0 - 65535" << std::endl;
		return ;
	}

	// Create the server socket
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		std::cerr << "Error creating socket" << std::endl;
		return ;
	}
	// Defining server address
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(_port);
	_serverAddress.sin_addr.s_addr = INADDR_ANY;

	// Bind socket to address
	if (bind(_serverSocket, (struct sockaddr*)&_serverAddress, sizeof(_serverAddress)) < 0)
	{
		std::cerr << "Error binding socket address" << std::endl;
		return ;
	}

	// Listening for incoming connections
	if (listen(_serverSocket, 5) < 0)
	{
		std::cerr << "Error listening socket" << std::endl;
		return ;
	}

	// Set pollfd struct to use it as id for clients
	_activeClients = 0;
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		_pfds[i].fd = -1;
		_pfds[i].events = 0;
		_pfds[i].revents = 0;
	}
	_pfds[0].fd = _serverSocket;
	_pfds[0].events = POLLIN;

	// Set client data struct
	for (int i = 0; i < MAX_CLIENTS; i++)
		_data[i].fd = -1;
	_data[0].fd = _pfds[0].fd;
	_data[0].str = "Server Socket";

}

void	Ircserv::servLoop()
{
	// Poll and accept client connection in loop
	//int	clientSocket;
	int ready;
	while (1)
	{
		ready = poll(_pfds, MAX_CLIENTS, -1);
		if (ready < 0)
		{
			std::cerr << "Error in poll" << std::endl;
			return ;
		}

		// Adding new client
		addNewClient();
		

		std::cout << "LOOP" << std::endl;

		// Work with existing clients
		existClient();
		
	}
}

void	Ircserv::addNewClient()
{
	if (_pfds[0].revents & POLLIN)
	{
		sockaddr_in	clientAddress;
		socklen_t	addr_len = sizeof(clientAddress);
		_clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddress, &addr_len);
		if (_clientSocket != -1)
		{
			int	added = 0;
			for (int i = 1; i < MAX_CLIENTS; i++)
			{
				if (_pfds[i].fd == -1)
				{
					_pfds[i].fd = _clientSocket;
					_pfds[i].events = POLLIN;
					_pfds[i].revents = 0;
					_data[i].fd = _pfds[i].fd;
					added = 1;
					std::cout << "New client on the slot " << _clientSocket << std::endl;
					_activeClients++;
					break ;
				}
			}
			if (added == 0)
			{
				std::cout << "Server is full, client denied" << std::endl;
				close(_clientSocket);
			}
		}
	}
}

void	Ircserv::existClient()
{
	for (int i = 1; i < MAX_CLIENTS; i++)
	{
		if (_pfds[i].fd == -1)
			continue ;
		if (_pfds[i].revents & POLLIN)
		{
			char	buffer[1024];
			bzero(buffer, 1024);
			ssize_t recBite = recv(_pfds[i].fd, buffer, sizeof(buffer) - 1, 0);
			if (recBite == -1)
			{
				std::cout << "Error reading from socket " << _pfds[i].fd << std::endl;
				close(_pfds[i].fd);
				_pfds[i].fd = -1;
				close(_data[i].fd);
				_data[i].fd = -1;
				_data[i].str.clear();
				_activeClients--;
			}
			else if (recBite == 0)
			{
				std::cout << "Client disconect on fd " << _pfds[i].fd << std::endl;
				close(_pfds[i].fd);
				_pfds[i].fd = -1;
				close(_data[i].fd);
				_data[i].fd = -1;
				_data[i].str.clear();
				_activeClients--;
			}
			else
			{
				// Use write to send message back to client on fd (_pfds[i].fd)
				//int n;
				//n = write(_pfds[i].fd, "I got your message", 18);
				//if (n < 0)
				//	std::cout << "Error writing client" << std::endl;
				//else
				//	std::cout << "Message from client fd " << _pfds[i].fd << "--> " << buffer << std::endl;

				_data[i].str = buffer;
				std::cout << "Message from client fd " << _pfds[i].fd << "--> " << buffer << std::endl;
			}
		}
	}
}
