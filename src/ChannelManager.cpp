
#include "ChannelManager.hpp"
#include "Client.hpp"
#include "Replies.hpp"

ChannelManager::ChannelManager()
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

std::string ChannelManager::prefix(Client *c) const
{
	return (":" + c->getNick() + "!" + c->getUser() + "@localhost");
}

// 353 + 366: who is in the channel, client needs both to show the user list
void ChannelManager::sendNames(Client *c, Channel *ch)
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

// last one left -> channel is gone
void ChannelManager::deleteIfEmpty(Channel *ch)
{
	if (!ch->isEmpty())
		return;
	std::string name = ch->getName(); // copy first, erase destroys ch (and its name)
	_channels.erase(name);
}
