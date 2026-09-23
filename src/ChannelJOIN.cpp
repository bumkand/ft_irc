
#include "ChannelManager.hpp"
#include "Client.hpp"
#include "Replies.hpp"

// JOIN Command:
// JOIN #chan[,#chan2] [key[,key2]]
// join() splits the lists, joinOne() does one channel, canJoin() checks +i +k +l

void ChannelManager::join(Client *c, const std::vector<std::string> &params)
{
	if (params.empty() || params[0].empty())
	{
		c->sendMsg(ERR_NEEDMOREPARAMS(c->getNick(), "JOIN"));
		return;
	}
	std::vector<std::string> names = splitComma(params[0]);
	std::vector<std::string> keys;
	if (params.size() > 1)
		keys = splitComma(params[1]);

	size_t i = 0;
	while (i < names.size())
	{
		std::string key = ""; // 1st key goes with 1st channel, 2nd with 2nd...
		if (i < keys.size())
			key = keys[i];
		joinOne(c, names[i], key);
		i++;
	}
}

// checks +i +k +l. if he can't get in, sends the error and returns false
bool ChannelManager::canJoin(Client *c, Channel *ch, const std::string &key)
{
	std::string nick = c->getNick();
	std::string name = ch->getName();

	if (ch->isInviteOnly() && !ch->isInvited(c))
	{
		c->sendMsg(ERR_INVITEONLYCHAN(nick, name));
		return (false);
	}
	if (!ch->getKey().empty() && key != ch->getKey())
	{
		c->sendMsg(ERR_BADCHANNELKEY(nick, name));
		return (false);
	}
	if (ch->getUserLimit() > 0 && (int)ch->memberCount() >= ch->getUserLimit())
	{
		c->sendMsg(ERR_CHANNELISFULL(nick, name));
		return (false);
	}
	return (true);
}

// one channel at a time
void ChannelManager::joinOne(Client *c, const std::string &name, const std::string &key)
{
	std::string nick = c->getNick();

	// channel name must be # + at least one char
	if (name.size() < 2 || name[0] != '#')
	{
		c->sendMsg(ERR_NOSUCHCHANNEL(nick, name));
		return;
	}
	Channel *ch = findChannel(name);
	if (ch == NULL)
	{
		// nobody here yet -> create it, first one in becomes op
		_channels.insert(std::make_pair(name, Channel(name)));
		ch = findChannel(name);
		ch->addMember(c);
		ch->addOperator(c);
	}
	else
	{
		if (ch->isMember(c))
			return; // already in, nothing to do
		if (!canJoin(c, ch, key))
			return; // error already sent
		ch->addMember(c);
	}
	// everyone sees the JOIN, the joiner too (that's how his client opens the window)
	ch->broadcast(prefix(c) + " JOIN " + name + "\r\n", NULL);
	if (!ch->getTopic().empty())
		c->sendMsg(RPL_TOPIC(nick, name, ch->getTopic()));
	sendNames(c, ch);
}
