#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>

// MOCK client - only for testing my channel code
// kata's real Client will replace this, same function names

class Client
{
private:
	int _fd;
	std::string _nick;
	std::string _user;
	bool _registered;
	std::string _outBuf; // what the server would send to this client

public:
	Client(int fd, const std::string &nick, const std::string &user)
		: _fd(fd), _nick(nick), _user(user), _registered(true) {}

	int getFd() const { return _fd; }
	const std::string &getNick() const { return _nick; }
	const std::string &getUser() const { return _user; }
	bool isRegistered() const { return _registered; }

	// real one will just add to the buffer, jakub sends it on POLLOUT
	void sendMsg(const std::string &msg)
	{
		_outBuf += msg;
		std::cout << "[to " << _nick << "] " << msg; // so i see it in terminal
	}

	// only for tests: check what he got, then reset
	const std::string &getOutBuf() const { return _outBuf; }
	void clearOutBuf() { _outBuf.clear(); }
};

#endif
