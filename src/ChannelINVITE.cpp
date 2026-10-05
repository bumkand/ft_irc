
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"

// INVITE Command:
// INVITE nick #chan
// inviter must be in the channel. if +i, he also has to be op
// target gets put on the invite list -> he can JOIN even with +i on

// with +i only ops can invite + no point inviting someone who's already inside
static bool canInvite(ClientData *c, Channel *ch, ClientData *target)
{
	if (ch->isInviteOnly() && !ch->isOperator(c))
	{
		c->sendMsg(ERR_CHANOPRIVSNEEDED(c->getNick(), ch->getName()));
		return (false);
	}
	if (ch->isMember(target))
	{
		c->sendMsg(ERR_USERONCHANNEL(c->getNick(), target->getNick(), ch->getName()));
		return (false);
	}
	return (true);
}

void ChannelManager::invite(ClientData *c, const std::vector<std::string> &params)
{
	if (params.size() < 2 || params[0].empty() || params[1].empty())
	{
		c->sendMsg(ERR_NEEDMOREPARAMS(c->getNick(), "INVITE"));
		return;
	}
	// target is NOT in the channel yet -> look in the whole server, not in members
	ClientData *target = findClient(params[0]);
	if (target == NULL)
	{
		c->sendMsg(ERR_NOSUCHNICK(c->getNick(), params[0]));
		return;
	}
	Channel *ch = getMyChannel(c, params[1]);
	if (ch == NULL || !canInvite(c, ch, target))
		return; // error already sent
	ch->addInvite(target); // used up when he leaves (removeMember drops it)
	c->sendMsg(RPL_INVITING(c->getNick(), target->getNick(), ch->getName())); // 341 = "ok, invite sent"
	target->sendMsg(prefix(c) + " INVITE " + target->getNick() + " :" + ch->getName() + "\r\n");
}
