
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"
#include <sstream>

// MODE Command:
// MODE #chan                    -> show modes (324)
// MODE #chan +itk-l secret      -> change them, only ops
// i = invite only, t = only ops change topic, k = key, l = user limit, o = give/take op
// args are taken in order: +k key, +l number, +o / -o nick. -k and -l take nothing
// the single modes (i t k l o) live in ChannelMODEflags.cpp

// does this mode eat the next argument?
static bool needsArg(char m, char sign)
{
	if (m == 'o')
		return (true);
	if ((m == 'k' || m == 'l') && sign == '+')
		return (true);
	return (false);
}

// 324: "+itkl key 5". key only shown to members, others don't need to know it
static void sendModes(ClientData *c, Channel *ch)
{
	std::string args = "";

	if (!ch->getKey().empty() && ch->isMember(c))
		args += " " + ch->getKey();
	if (ch->getUserLimit() > 0)
	{
		std::stringstream ss;
		ss << ch->getUserLimit();
		args += " " + ss.str();
	}
	c->sendMsg(RPL_CHANNELMODEIS(c->getNick(), ch->getName(), ch->modeString() + args));
}

// "MODE #c" or "MODE #c b" only ask, they don't change anything -> anyone can do it
static bool isQuery(ClientData *c, Channel *ch, const std::vector<std::string> &params)
{
	if (params.size() == 1)
		sendModes(c, ch);
	else if (params[1] == "b") // irssi asks for the ban list on join -> "list is empty"
		c->sendMsg(RPL_ENDOFBANLIST(c->getNick(), ch->getName()));
	else
		return (false);
	return (true);
}

// walk "+it-l", keep track of + / -, give each mode its arg
void ChannelManager::applyModes(ClientData *c, Channel *ch,
								const std::vector<std::string> &params, ModeChanges &out)
{
	const std::string &modes = params[1];
	size_t argIdx = 2; // params[2] = first arg
	char sign = '+'; // "MODE #c i" = "+i"
	size_t i = 0;

	while (i < modes.size())
	{
		char m = modes[i++];
		std::string arg = "";
		if (m == '+' || m == '-')
		{
			sign = m;
			continue;
		}
		if (needsArg(m, sign))
		{
			if (argIdx >= params.size())
			{
				c->sendMsg(ERR_NEEDMOREPARAMS(c->getNick(), "MODE"));
				continue; // skip only this mode, the rest still works
			}
			arg = params[argIdx++];
		}
		applyOne(c, ch, sign, m, arg, out);
	}
}

void ChannelManager::mode(ClientData *c, const std::vector<std::string> &params)
{
	if (params.empty() || params[0].empty())
	{
		c->sendMsg(ERR_NEEDMOREPARAMS(c->getNick(), "MODE"));
		return;
	}
	if (params[0][0] != '#')
		return; // user modes (irssi sends "MODE alice +i" on connect) -> not our job
	Channel *ch = findChannel(params[0]);
	if (ch == NULL)
	{
		c->sendMsg(ERR_NOSUCHCHANNEL(c->getNick(), params[0]));
		return;
	}
	if (isQuery(c, ch, params))
		return; // just asked, nothing to change
	if (!ch->isOperator(c))
	{
		c->sendMsg(ERR_CHANOPRIVSNEEDED(c->getNick(), ch->getName()));
		return;
	}
	ModeChanges out;
	out.lastSign = 0;
	applyModes(c, ch, params, out);
	if (!out.modes.empty()) // everyone sees what changed (op too)
		ch->broadcast(prefix(c) + " MODE " + ch->getName() + " " + out.modes + out.args + "\r\n", NULL);
}
