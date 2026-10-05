
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"

// PRIVMSG / NOTICE Command (channel part only):
// PRIVMSG #chan :text
// kata already checked 411/412 and split "#a,#b", she calls this once per #target
// notice = true -> NOTICE: same thing but never send errors back (RFC rule)

void ChannelManager::privmsg(ClientData *c, const std::string &target,
								const std::string &text, bool notice)
{
	std::string cmd = "PRIVMSG";
	if (notice)
		cmd = "NOTICE";

	Channel *ch = findChannel(target);
	if (ch == NULL)
	{
		if (!notice)
			c->sendMsg(ERR_NOSUCHCHANNEL(c->getNick(), target));
		return;
	}
	if (!ch->isMember(c))
	{
		if (!notice)
			c->sendMsg(ERR_CANNOTSENDTOCHAN(c->getNick(), target));
		return;
	}
	// everyone except the sender, his client already shows what he typed
	ch->broadcast(prefix(c) + " " + cmd + " " + target + " :" + text + "\r\n", c);
}
