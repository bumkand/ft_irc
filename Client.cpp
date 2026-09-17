#include "includes/Client.hpp"

int main(void)
{
	// Create client socket
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Defining server address
	sockaddr_in	serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	// Connect to server
	connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	std::string	message;
	while (1)
	{
		// Send data to the server
		std::cin >> message;
		send(clientSocket, message.c_str(), strlen(message.c_str()), 0);

		// Read from socket
		//int n;
		//char	buffer[1024];
		//bzero(buffer, 1024);
		//n = read(clientSocket, buffer, 1024);
		//if (n < 0)
		//	std::cout << "Error reading from socket" << std::endl;
		//else
		//	std::cout << buffer << std::endl;
	}


	// Close client socket
	close(clientSocket);
}
