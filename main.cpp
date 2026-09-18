#include "includes/Ircserv.hpp"

int	main(int arc, char *arv[])
{
	if (arc != 2)
		return std::cout << "Wrong amount of arguments" << std::endl, 1;
	
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
	

	
	//close(_serverSocket);
	return 0;
}
