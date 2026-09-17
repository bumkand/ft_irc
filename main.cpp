#include "includes/Ircserv.hpp"

int	main(int arc, char *arv[])
{
	if (arc != 2)
		return std::cout << "Wrong amount of arguments" << std::endl, 1;
	
	Ircserv	irc;
	irc.initServ(arv);
	irc.servLoop();
	

	
	//close(_serverSocket);
	return 0;
}
