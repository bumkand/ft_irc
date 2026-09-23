
#include "Channel.hpp"
#include "Client.hpp" // real one or the mock, depends on -I order

Channel::Channel(const std::string &name)
	: _name(name),
	_topic(""),
	_key(""),
	_inviteOnly(false),
	_topicOpOnly(false),
	_userLimit(0)
{
}

Channel::~Channel()
{
	// we don't own the Client pointers, the server does -> nothing to delete
}

// small helper: is c in this vector?
static bool contains(const std::vector<Client *> &v, Client *c)
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
static void removeFrom(std::vector<Client *> &v, Client *c)
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

bool Channel::isMember(Client *c) const { return (contains(_members, c)); }
bool Channel::isOperator(Client *c) const { return (contains(_operators, c)); }
bool Channel::isInvited(Client *c) const { return (contains(_invited, c)); }
bool Channel::isEmpty() const { return (_members.empty()); }
size_t Channel::memberCount() const { return (_members.size()); }

void Channel::addMember(Client *c)
{
	if (!isMember(c)) // no duplicates
		_members.push_back(c);
}

void Channel::removeMember(Client *c)
{
	removeFrom(_members, c);
	removeFrom(_operators, c); // not in channel -> can't be op either
	removeFrom(_invited, c); // invite is used up
}

void Channel::addOperator(Client *c)
{
	if (!isOperator(c))
		_operators.push_back(c);
}

void Channel::removeOperator(Client *c)
{ 
	removeFrom(_operators, c);
}

void Channel::addInvite(Client *c)
{
	if (!isInvited(c))
		_invited.push_back(c);
}

void Channel::broadcast(const std::string &msg, Client *skip)
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
const std::vector<Client *> &Channel::getMembers() const { return (_members); }
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
