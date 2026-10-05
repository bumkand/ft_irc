
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"

// TOPIC Command:
// TOPIC #chan          -> just show it: 332 (topic) or 331 (no topic)
// TOPIC #chan :text    -> change it (if +t only ops can), everyone in the channel sees it
// TOPIC #chan :        -> empty text = topic gets cleared

// only the channel name given > he just wants to see the topic
static void showTopic(ClientData *c, Channel *ch)
{
	if (ch->getTopic().empty())
		c->sendMsg(RPL_NOTOPIC(c->getNick(), ch->getName()));
	else
		c->sendMsg(RPL_TOPIC(c->getNick(), ch->getName(), ch->getTopic()));
}

void ChannelManager::topic(ClientData *c, const std::vector<std::string> &params)
{
	if (params.empty() || params[0].empty())
	{
		c->sendMsg(ERR_NEEDMOREPARAMS(c->getNick(), "TOPIC"));
		return;
	}
	Channel *ch = getMyChannel(c, params[0]);
	if (ch == NULL)
		return; // 403 / 442 already sent
	if (params.size() == 1)
	{
		showTopic(c, ch);
		return;
	}
	if (ch->isTopicOpOnly() && !ch->isOperator(c)) // +t on -> only ops can change it
	{
		c->sendMsg(ERR_CHANOPRIVSNEEDED(c->getNick(), ch->getName()));
		return;
	}
	ch->setTopic(params[1]);
	// everyone sees it, the setter too (that's how his client updates the topic bar)
	ch->broadcast(prefix(c) + " TOPIC " + ch->getName() + " :" + params[1] + "\r\n", NULL);
}
