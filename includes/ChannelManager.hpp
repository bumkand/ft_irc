
#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include "Channel.hpp"

class ClientData;

// what one MODE command really changed, sent to the channel at the end
struct ModeChanges
{
	std::string modes; // "+it-k"
	std::string args;  // " secret bob"
	char lastSign;     // last + / - written into modes
};

// owns all channels + runs the channel commands (JOIN, PART, KICK, ...)
// kata's dispatch calls these, params = everything after the command name
class ChannelManager
{
private:
	std::map<std::string, Channel> _channels; // "#name" in lowercase -> the channel (#Test == #test)
	ClientData *_clients; // jakub's _data array, to find people who are in NO channel (INVITE)
	int _clientCount;    // size of that array (MAX_CLIENTS)

	std::string prefix(ClientData *c) const; // ":nick!user@host"
	void sendNames(ClientData *c, Channel *ch);
	std::vector<std::string> splitComma(const std::string &str) const;
	void deleteIfEmpty(Channel *ch);
	ClientData *findMember(Channel *ch, const std::string &nick) const; // NULL if he's not in ch
	ClientData *findClient(const std::string &nick) const; // whole server, NULL if no such nick
	Channel *getMyChannel(ClientData *c, const std::string &name); // exists + c is in it, else 403/442 + NULL
	ClientData *kickTarget(ClientData *c, Channel *ch, const std::string &nick); // op check + find target
	bool canJoin(ClientData *c, Channel *ch, const std::string &key);
	void joinOne(ClientData *c, const std::string &name, const std::string &key);
	void partOne(ClientData *c, const std::string &name, const std::string &reason);

	void whoChannel(ClientData *c, const std::string &name); // ChannelWHO.cpp

	// MODE helpers (ChannelMODE.cpp)
	void applyModes(ClientData *c, Channel *ch, const std::vector<std::string> &params, ModeChanges &out);
	void applyOne(ClientData *c, Channel *ch, char sign, char m, const std::string &arg, ModeChanges &out);
	void modeOp(ClientData *c, Channel *ch, char sign, const std::string &arg, ModeChanges &out);

public:
	ChannelManager();
	~ChannelManager();

	// called once by the server, so I can search all clients
	void setClients(ClientData *clients, int count);

	Channel *findChannel(const std::string &name); // NULL if it doesn't exist

	// commands
	void join(ClientData *c, const std::vector<std::string> &params);
	void part(ClientData *c, const std::vector<std::string> &params);
	// one #target at a time, kata loops over "#a,#b". notice = true -> no error replies
	void privmsg(ClientData *c, const std::string &target, const std::string &text, bool notice);

	void topic(ClientData *c, const std::vector<std::string> &params);
	void kick(ClientData *c, const std::vector<std::string> &params);
	void invite(ClientData *c, const std::vector<std::string> &params);
	void mode(ClientData *c, const std::vector<std::string> &params);
	void who(ClientData *c, const std::vector<std::string> &params);

	// send msg once to everyone who shares a channel with c (not c). QUIT + NICK use it
	void sendToShared(ClientData *c, const std::string &msg);

	// QUIT or lost connection: call BEFORE resetClient(), the slot gets reused
	void removeClient(ClientData *c, const std::string &reason);
};

#endif
