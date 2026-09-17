
#include <iostream>
#include <cstdlib>
#include <csignal>
#include "Server.hpp"

// global flag so the signal handler can tell the poll loop to stop
// (Role 1 - Jakub: the run() loop checks this every iteration)
bool g_running = true;

// ctrl+c -> just flip the flag, let run() exit cleanly and close every fd
static void handle_sigint(int sig)
{
	(void)sig;
	g_running = false;
}

// port has to be a number in 1..65535, nothing else
static bool valid_port(const std::string &str)
{
	if (str.empty() || str.size() > 5)
		return (false);
	for (size_t i = 0; i < str.size(); i++)
		if (!isdigit(str[i]))
			return (false);
	int port = atoi(str.c_str());
	return (port > 0 && port <= 65535);
}

int main(int argc, char **argv)
{
	// usage: ./ircserv <port> <password>
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	if (!valid_port(argv[1]) || std::string(argv[2]).empty())
	{
		std::cerr << "Error: bad port or empty password" << std::endl;
		return (1);
	}
	signal(SIGINT, handle_sigint);
	signal(SIGPIPE, SIG_IGN); // writing to a dead client must not kill us

	// ---- Role 1 (Jakub) - Server ctor ----
	// socket() + setsockopt(SO_REUSEADDR) + bind() + listen()
	// fcntl(fd, F_SETFL, O_NONBLOCK) on the listen socket
	// push the listen fd as the first entry of the pollfd vector
	Server server(atoi(argv[1]), argv[2]);

	// ---- Role 1 (Jakub) - Server::run ----
	// while (g_running): poll() on all fds
	//   listen fd readable  -> accept(), set O_NONBLOCK, new Client (Role 2)
	//   client fd readable  -> recv() into that client's buffer,
	//                          cut on "\r\n", hand each full line to Role 2
	//   recv() == 0 / error -> disconnect, Role 3 removes him from channels
	// ---- Role 2 (Kata) - called from run() per full line ----
	// parse line -> command + params, then dispatch:
	//   PASS / NICK / USER  -> registration (must be done in this order)
	//   PRIVMSG to a nick   -> direct message between two registered users
	//   anything else before registered -> error reply, ignore
	// ---- Role 3 (Jasmine) - called from Role 2's dispatch ----
	//   JOIN / PART          -> create channel if missing, add/remove member
	//   PRIVMSG to #channel  -> broadcast to every member except sender
	//   KICK / INVITE / TOPIC -> operator only
	//   MODE i t k o l        -> toggle flags on the Channel object
	server.run();

	// run() returned because g_running went false -> Server dtor closes fds
	return (0);
}

// It assumes Server(int port, const std::string &password) and Server::run() exist 
// — that's Jakub's class, so the names should match whatever he picks. 
// If he goes with a different constructor signature, only that one line changes.

// g_running is a global on purpose: a signal handler can't take arguments, 
// so the poll loop in run() reads it as its while condition. 
// SIGPIPE is ignored so that a send() to a client who already disconnected doesn't kill the whole server.

// The comment block before server.run() is the role map: 
// it lists exactly what each of you has to put in the functions that main
// never calls directly (accept/recv/CRLF splitting for Jakub, parse 
// + PASS/NICK/USER + PRIVMSG-to-nick for Kata, JOIN/PART/broadcast/KICK/INVITE/TOPIC/MODE for you).