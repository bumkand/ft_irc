#ifndef CLIENTDATA_HPP
#define CLIENTDATA_HPP

#include <string>
#include <iostream>

// MOCK client - only for testing my channel code
// real one is includes/ClientData.hpp, same function names

class ClientData
{
private:
	int _fd;
	std::string _nick;
	std::string _user;
	bool _registered;
	std::string _outBuf; // what the server would send to this client

public:
	ClientData(int fd, const std::string &nick, const std::string &user)
		: _fd(fd), _nick(nick), _user(user), _registered(true) {}

	int getFD() const { return _fd; }
	const std::string &getNick() const { return _nick; }
	const std::string &getUsername() const { return _user; }
	bool isRegistered() const { return _registered; }
	std::string getFullMask() const { return _nick + "!" + _user + "@localhost"; } // real one uses the IP

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

// simple copy of kata's my_tolower (utils.cpp), so findMember works in tests
inline std::string my_tolower(const std::string &str)
{
	std::string out = str;
	size_t i = 0;

	while (i < out.size())
	{
		if (out[i] >= 'A' && out[i] <= 'Z')
			out[i] = out[i] + ('a' - 'A');
		i++;
	}
	return (out);
}

#endif
