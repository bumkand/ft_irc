#include "Ircserv.hpp"

volatile sig_atomic_t	checkSig = 0;

int	main(int arc, char *arv[])
{
	if (arc != 3)
		return std::cerr << "Wrong amount of arguments" << std::endl, 1;
	if (strlen(arv[1]) == 0 || strlen(arv[1]) > 5)
		return std::cerr << "Invalid port number" << std::endl, 1;
	for (size_t i = 0; i < strlen(arv[1]); i++)
	{
		if (std::isdigit(arv[1][i]))
			continue ;
		else
			return std::cerr << "Invalid port number" << std::endl, 1;
	}
	if (strlen(arv[2]) == 0)
		return std::cerr << "Missing password" << std::endl, 1;
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGPIPE, SIG_IGN);

	bool success;
	try
	{
		Ircserv	irc;
		irc.initServ(arv);
		std::cout << "Server inicialized and listening" << std::endl;
		success = irc.servLoop();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	if (success)
	{
		return 0;
	}
	return 1;
}
