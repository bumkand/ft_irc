
#include "ChannelManager.hpp"
#include "Client.hpp"
#include "Replies.hpp"

// PART Command:
// PART #chan[,#chan2] [:reason]
// part() splits the list, partOne() does one channel

void ChannelManager::part(Client *c, const std::vector<std::string> &params)
{
	if (params.empty() || params[0].empty())
	{
		c->sendMsg(ERR_NEEDMOREPARAMS(c->getNick(), "PART"));
		return;
	}
	std::string reason = ""; // optional, same reason for every channel
	if (params.size() > 1)
		reason = params[1];

	std::vector<std::string> names = splitComma(params[0]);
	size_t i = 0;
	while (i < names.size())
	{
		partOne(c, names[i], reason);
		i++;
	}
}

void ChannelManager::partOne(Client *c, const std::string &name, const std::string &reason)
{
	Channel *ch = findChannel(name);

	if (ch == NULL)
	{
		c->sendMsg(ERR_NOSUCHCHANNEL(c->getNick(), name));
		return;
	}
	if (!ch->isMember(c))
	{
		c->sendMsg(ERR_NOTONCHANNEL(c->getNick(), name));
		return;
	}
	std::string msg = prefix(c) + " PART " + name;
	if (!reason.empty())
		msg += " :" + reason;
	// send BEFORE removing him, so he gets it too (his client closes the window)
	ch->broadcast(msg + "\r\n", NULL);
	ch->removeMember(c);
	deleteIfEmpty(ch); // ch can be gone after this, don't use it anymore
}
