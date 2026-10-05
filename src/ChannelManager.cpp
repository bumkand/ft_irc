
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"

ChannelManager::ChannelManager(): _clients(NULL), _clientCount(0)
{
}

ChannelManager::~ChannelManager()
{
	// channels are stored by value in the map > map cleans them up itself
}

Channel *ChannelManager::findChannel(const std::string &name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(name);

	if (it == _channels.end())
		return (NULL);
	return (&it->second); // address of the channel inside the map
}

std::string ChannelManager::prefix(ClientData *c) const
{
	return (":" + c->getFullMask()); // same "nick!user@host" kata uses
}

// 353 + 366: who is in the channel, client needs both to show the user list
void ChannelManager::sendNames(ClientData *c, Channel *ch)
{
	c->sendMsg(RPL_NAMREPLY(c->getNick(), ch->getName(), ch->namesList()));
	c->sendMsg(RPL_ENDOFNAMES(c->getNick(), ch->getName()));
}

// "#a,#b,#c" > "#a" "#b" "#c" (JOIN and PART both take lists)
std::vector<std::string> ChannelManager::splitComma(const std::string &str) const
{
	std::vector<std::string> out;
	std::string part;
	size_t i = 0;

	while (i < str.size())
	{
		if (str[i] == ',')
		{
			out.push_back(part);
			part.clear();
		}
		else
			part += str[i];
		i++;
	}
	out.push_back(part); // last one has no comma after it
	return (out);
}

// look for a nick among the members. nicks don't care about case (BOB == bob)
ClientData *ChannelManager::findMember(Channel *ch, const std::string &nick) const
{
	const std::vector<ClientData *> &members = ch->getMembers();
	size_t i = 0;

	while (i < members.size())
	{
		if (my_tolower(members[i]->getNick()) == my_tolower(nick))
			return (members[i]);
		i++;
	}
	return (NULL);
}

void ChannelManager::setClients(ClientData *clients, int count)
{
	_clients = clients;
	_clientCount = count;
}

// look for a nick in the whole server. skip empty slots + people not registered yet
ClientData *ChannelManager::findClient(const std::string &nick) const
{
	int i = 0;

	while (i < _clientCount)
	{
		if (_clients[i].getFD() != -1 && _clients[i].isRegistered()
			&& my_tolower(_clients[i].getNick()) == my_tolower(nick))
			return (&_clients[i]);
		i++;
	}
	return (NULL);
}

// most commands need: channel exists (403) + you're in it (442). NULL = error already sent
Channel *ChannelManager::getMyChannel(ClientData *c, const std::string &name)
{
	Channel *ch = findChannel(name);

	if (ch == NULL)
	{
		c->sendMsg(ERR_NOSUCHCHANNEL(c->getNick(), name));
		return (NULL);
	}
	if (!ch->isMember(c))
	{
		c->sendMsg(ERR_NOTONCHANNEL(c->getNick(), name));
		return (NULL);
	}
	return (ch);
}

// last one left -> channel is gone
void ChannelManager::deleteIfEmpty(Channel *ch)
{
	if (!ch->isEmpty())
		return;
	std::string name = ch->getName(); // copy first, erase destroys ch (and its name)
	_channels.erase(name);
}
