#include "Ircserv.hpp"

volatile sig_atomic_t	checkSig = 0;

int	main(int arc, char *arv[])
{
	if (arc != 3)
		return std::cerr << "Wrong amount of arguments" << std::endl, 1;
	signal(SIGINT, signalHandler);

	try
	{
		Ircserv	irc;
		irc.initServ(arv);
		std::cout << "Server inicialized and listening" << std::endl;
		irc.servLoop();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	

	return 0;
}
