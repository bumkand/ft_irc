
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"
#include <sstream>
#include <cstdlib>

// MODE part 2: what each mode letter actually does (i t k l o)
// ChannelMODE.cpp walks the "+itk-l" string and calls applyOne() for every letter

// remember one change, so at the end everyone gets ONE line like "+it-k"
static void addChange(ModeChanges &out, char sign, char m, const std::string &arg)
{
	if (out.lastSign != sign) // "+i+t" -> "+it", sign only when it changes
	{
		out.modes += sign;
		out.lastSign = sign;
	}
	out.modes += m;
	if (!arg.empty())
		out.args += " " + arg;
}

// i and t are just on / off
static void modeFlag(Channel *ch, char sign, char m, ModeChanges &out)
{
	bool on = (sign == '+');
	bool now;

	if (m == 'i')
		now = ch->isInviteOnly();
	else
		now = ch->isTopicOpOnly();
	if (on == now)
		return; // already like that -> nothing to announce
	if (m == 'i')
		ch->setInviteOnly(on);
	else
		ch->setTopicOpOnly(on);
	addChange(out, sign, m, "");
}

// +k key / -k
static void modeKey(Channel *ch, char sign, const std::string &arg, ModeChanges &out)
{
	if (sign == '+')
	{
		ch->setKey(arg);
		addChange(out, '+', 'k', arg);
	}
	else if (!ch->getKey().empty())
	{
		ch->setKey("");
		addChange(out, '-', 'k', "");
	}
}

// +l number / -l
static void modeLimit(Channel *ch, char sign, const std::string &arg, ModeChanges &out)
{
	if (sign == '+')
	{
		int n = std::atoi(arg.c_str());
		if (n <= 0)
			return; // "abc" or "0" -> makes no sense, ignore
		std::stringstream ss;
		ss << n; // back to text, so "05" is sent as "5"
		ch->setUserLimit(n);
		addChange(out, '+', 'l', ss.str());
	}
	else if (ch->getUserLimit() > 0)
	{
		ch->setUserLimit(0);
		addChange(out, '-', 'l', "");
	}
}

// +o nick / -o nick, target has to be in the channel
void ChannelManager::modeOp(ClientData *c, Channel *ch, char sign,
							const std::string &arg, ModeChanges &out)
{
	ClientData *target = findMember(ch, arg);

	if (target == NULL)
	{
		c->sendMsg(ERR_USERNOTINCHANNEL(c->getNick(), arg, ch->getName()));
		return;
	}
	if (sign == '+' && !ch->isOperator(target))
		ch->addOperator(target);
	else if (sign == '-' && ch->isOperator(target))
		ch->removeOperator(target);
	else
		return; // already op / already not op -> nothing changed
	addChange(out, sign, 'o', target->getNick());
}

// one mode letter > the right function
void ChannelManager::applyOne(ClientData *c, Channel *ch, char sign, char m,
							const std::string &arg, ModeChanges &out)
{
	if (m == 'i' || m == 't')
		modeFlag(ch, sign, m, out);
	else if (m == 'k')
		modeKey(ch, sign, arg, out);
	else if (m == 'l')
		modeLimit(ch, sign, arg, out);
	else if (m == 'o')
		modeOp(c, ch, sign, arg, out);
	else if (m != 'b') // b = ban list, not in the subject -> just skip it quietly
		c->sendMsg(ERR_UNKNOWNMODE(c->getNick(), m));
}
