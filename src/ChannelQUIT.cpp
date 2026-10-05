
#include "ChannelManager.hpp"
#include "ClientData.hpp"
#include "Replies.hpp"

// QUIT / disconnect:
// kata calls this on QUIT, jakub when recv() returns 0 / error.
// must happen BEFORE resetClient(): the slot gets reused by the next person
// who connects, and he would inherit this guy's channels + op rights

// is c already in the list?
static bool alreadyIn(const std::vector<ClientData *> &list, ClientData *c)
{
	size_t i = 0;

	while (i < list.size())
	{
		if (list[i] == c)
			return (true);
		i++;
	}
	return (false);
}

// send the QUIT line to everyone in ch who didn't get it yet
// (bob + alice in 3 same channels > alice gets 1 QUIT, not 3)
static void tellChannel(Channel *ch, ClientData *c,
						const std::string &msg,
						std::vector<ClientData *> &told)
{
	const std::vector<ClientData *> &members = ch->getMembers();
	size_t i;

	i = 0;
	while (i < members.size())
	{
		if (members[i] != c && !alreadyIn(told, members[i]))
		{
			members[i]->sendMsg(msg);
			told.push_back(members[i]);
		}
		i++;
	}
}

void ChannelManager::removeClient(ClientData *c, const std::string &reason)
{
	std::string msg = prefix(c) + " QUIT :" + reason + "\r\n";
	std::vector<ClientData *> told; // who already got the QUIT line
	std::vector<std::string> emptyChannels; // erase them after the loop
	std::map<std::string, Channel>::iterator it;
	Channel *ch;
	size_t i;

	it = _channels.begin();
	while (it != _channels.end())
	{
		ch = &it->second;
		if (ch->isMember(c))
			tellChannel(ch, c, msg, told);
		// every channel, not only his: he can also be on an invite list
		ch->removeMember(c);
		if (ch->isEmpty())
			emptyChannels.push_back(ch->getName());
		it++;
	}
	// erasing inside the loop above would break it, so do it here
	i = 0;
	while (i < emptyChannels.size())
	{
		_channels.erase(emptyChannels[i]);
		i++;
	}
}
