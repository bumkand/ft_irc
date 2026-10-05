
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"

// KICK Command:
// KICK #chan nick [:reason]
// only ops can kick. everyone sees the KICK line (the kicked one too), then he's out

// kicker has to be op + target has to be in the channel, NULL = error already sent
ClientData *ChannelManager::kickTarget(ClientData *c, Channel *ch, const std::string &nick)
{
	if (!ch->isOperator(c))
	{
		c->sendMsg(ERR_CHANOPRIVSNEEDED(c->getNick(), ch->getName()));
		return (NULL);
	}
	ClientData *target = findMember(ch, nick);
	if (target == NULL)
		c->sendMsg(ERR_USERNOTINCHANNEL(c->getNick(), nick, ch->getName()));
	return (target);
}

void ChannelManager::kick(ClientData *c, const std::vector<std::string> &params)
{
	if (params.size() < 2 || params[0].empty() || params[1].empty())
	{
		c->sendMsg(ERR_NEEDMOREPARAMS(c->getNick(), "KICK"));
		return;
	}
	Channel *ch = getMyChannel(c, params[0]);
	if (ch == NULL)
		return; // 403 / 442 already sent
	ClientData *target = kickTarget(c, ch, params[1]);
	if (target == NULL)
		return; // 482 / 441 already sent
	std::string reason = c->getNick(); // no reason given -> kicker's nick (that's what most servers do)
	if (params.size() > 2 && !params[2].empty())
		reason = params[2];
	// send BEFORE removing him, so he gets it too (his client closes the window)
	ch->broadcast(prefix(c) + " KICK " + ch->getName() + " " + target->getNick() + " :" + reason + "\r\n", NULL);
	ch->removeMember(target); // also takes his op + invite
	deleteIfEmpty(ch); // op kicked himself and was alone -> channel gone, don't use ch after
}
