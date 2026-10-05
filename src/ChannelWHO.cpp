
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"

// WHO Command (simple version, no wildcards):
// WHO #chan   -> one 352 line per member, then 315
// WHO nick    -> one 352 line for him, then 315
// irssi sends "WHO #chan" after every join, without it irssi shows "421 Unknown command"

// one 352 line: H = here (not away), @ = channel op
static std::string whoLine(ClientData *asker, ClientData *who, const std::string &chan, bool op)
{
	std::string flags = "H";

	if (op)
		flags += "@";
	return (RPL_WHOREPLY(asker->getNick(), chan, who->getUsername(), who->getHost(),
			who->getNick(), flags, who->getRealname()));
}

// everyone in the channel. unknown channel -> nothing, 315 still comes after
void ChannelManager::whoChannel(ClientData *c, const std::string &name)
{
	Channel *ch = findChannel(name);
	size_t i = 0;

	if (ch == NULL)
		return;
	const std::vector<ClientData *> &members = ch->getMembers();
	while (i < members.size())
	{
		c->sendMsg(whoLine(c, members[i], ch->getName(), ch->isOperator(members[i])));
		i++;
	}
}

void ChannelManager::who(ClientData *c, const std::vector<std::string> &params)
{
	std::string mask = "*"; // "WHO" alone -> we just answer "end of list"
	if (!params.empty() && !params[0].empty())
		mask = params[0];

	if (mask[0] == '#')
		whoChannel(c, mask);
	else
	{
		ClientData *target = findClient(mask); // NULL for "*" too (findClient skips empty slots)
		if (target != NULL)
			c->sendMsg(whoLine(c, target, "*", false)); // "*" = not asking about a channel
	}
	c->sendMsg(RPL_ENDOFWHO(c->getNick(), mask)); // always, the client waits for it
}
