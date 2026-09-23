
#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include "Channel.hpp"

class Client;

// owns all channels + runs the channel commands (JOIN, PART, KICK, ...)
// kata's dispatch calls these, params = everything after the command name
class ChannelManager
{
private:
	std::map<std::string, Channel> _channels; // "#name" -> the channel itself

	std::string prefix(Client *c) const; // ":nick!user@localhost"
	void sendNames(Client *c, Channel *ch);
	std::vector<std::string> splitComma(const std::string &str) const;
	void deleteIfEmpty(Channel *ch);
	bool canJoin(Client *c, Channel *ch, const std::string &key);
	void joinOne(Client *c, const std::string &name, const std::string &key);
	void partOne(Client *c, const std::string &name, const std::string &reason);

public:
	ChannelManager();
	~ChannelManager();

	Channel *findChannel(const std::string &name); // NULL if it doesn't exist

	// commands
	void join(Client *c, const std::vector<std::string> &params);
	void part(Client *c, const std::vector<std::string> &params);
	void privmsg(Client *c, const std::vector<std::string> &params); // only for #targets

	// QUIT or lost connection: call BEFORE the client is deleted
	void removeClient(Client *c, const std::string &reason);
};

#endif
