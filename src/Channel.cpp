
#include "Channel.hpp"
#include "ClientData.hpp" // need the full class here, the .hpp only has "class ClientData;"

Channel::Channel(const std::string &name):
	_name(name),
	_topic(""),
	_key(""),
	_inviteOnly(false),
	_topicOpOnly(false),
	_userLimit(0)
{
}

Channel::~Channel()
{
	// we don't own the ClientData pointers, the server does > nothing to delete
}

// small helper: is c in this vector?
static bool contains(const std::vector<ClientData *> &v, ClientData *c)
{
	size_t i = 0;

	while (i < v.size())
	{
		if (v[i] == c)
			return (true);
		i++;
	}
	return (false);
}

// small helper: remove c from vector if he's there
static void removeFrom(std::vector<ClientData *> &v, ClientData *c)
{
	size_t i = 0;

	while (i < v.size())
	{
		if (v[i] == c)
		{
			v.erase(v.begin() + i);
			return;
		}
		i++;
	}
}

bool Channel::isMember(ClientData *c) const { return (contains(_members, c)); }
bool Channel::isOperator(ClientData *c) const { return (contains(_operators, c)); }
bool Channel::isInvited(ClientData *c) const { return (contains(_invited, c)); }
bool Channel::isEmpty() const { return (_members.empty()); }
size_t Channel::memberCount() const { return (_members.size()); }

void Channel::addMember(ClientData *c)
{
	if (!isMember(c)) // no duplicates
		_members.push_back(c);
}

void Channel::removeMember(ClientData *c)
{
	removeFrom(_members, c);
	removeFrom(_operators, c); // not in channel -> can't be op either
	removeFrom(_invited, c); // invite is used up
}

void Channel::addOperator(ClientData *c)
{
	if (!isOperator(c))
		_operators.push_back(c);
}

void Channel::removeOperator(ClientData *c)
{ 
	removeFrom(_operators, c);
}

void Channel::addInvite(ClientData *c)
{
	if (!isInvited(c))
		_invited.push_back(c);
}

void Channel::broadcast(const std::string &msg, ClientData *skip)
{
	size_t i = 0;

	while (i < _members.size())
	{
		if (_members[i] != skip)
			_members[i]->sendMsg(msg);
		i++;
	}
}

std::string Channel::namesList() const
{
	std::string out;
	size_t i = 0;

	while (i < _members.size())
	{
		if (i > 0)
			out += " ";
		if (isOperator(_members[i]))
			out += "@"; // ops get @ in front, that's how the client shows them
		out += _members[i]->getNick();
		i++;
	}
	return (out);
}

std::string Channel::modeString() const
{
	std::string out = "+";

	if (_inviteOnly)
		out += "i"; // invite-only
	if (_topicOpOnly)
		out += "t"; // topic restrictions
	if (!_key.empty())
		out += "k"; //. key/password
	if (_userLimit > 0)
		out += "l"; // user limits
	return (out);
}

const std::string &Channel::getName() const { return (_name); }
const std::vector<ClientData *> &Channel::getMembers() const { return (_members); }
const std::string &Channel::getTopic() const { return (_topic); }
const std::string &Channel::getKey() const { return (_key); }

bool Channel::isInviteOnly() const { return (_inviteOnly); }
bool Channel::isTopicOpOnly() const { return (_topicOpOnly); }

int Channel::getUserLimit() const { return (_userLimit); }

void Channel::setTopic(const std::string &t) { _topic = t; }
void Channel::setKey(const std::string &k) { _key = k; }
void Channel::setInviteOnly(bool on) { _inviteOnly = on; }
void Channel::setTopicOpOnly(bool on) { _topicOpOnly = on; }
void Channel::setUserLimit(int n) { _userLimit = n; }
