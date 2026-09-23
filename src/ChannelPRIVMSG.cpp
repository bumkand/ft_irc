
#include "ChannelManager.hpp"
#include "Client.hpp"
#include "Replies.hpp"

// PRIVMSG Command (channel part only):
// PRIVMSG #chan :text
// kata handles PRIVMSG to a nick, she calls this only when target starts with #

void ChannelManager::privmsg(Client *c, const std::vector<std::string> &params)
{
	if (params.empty())
	{
		c->sendMsg(ERR_NORECIPIENT(c->getNick(), "PRIVMSG"));
		return;
	}
	if (params.size() < 2 || params[1].empty())
	{
		c->sendMsg(ERR_NOTEXTTOSEND(c->getNick()));
		return;
	}
	std::string name = params[0];
	Channel *ch = findChannel(name);
	if (ch == NULL)
	{
		c->sendMsg(ERR_NOSUCHCHANNEL(c->getNick(), name));
		return;
	}
	if (!ch->isMember(c))
	{
		c->sendMsg(ERR_CANNOTSENDTOCHAN(c->getNick(), name));
		return;
	}
	// everyone except the sender, his client already shows what he typed
	ch->broadcast(prefix(c) + " PRIVMSG " + name + " :" + params[1] + "\r\n", c);
}
