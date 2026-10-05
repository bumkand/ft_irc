
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

class ClientData; // only pointers here, so no need to include the whole class

// one object per channel. only data + small rules, no command logic here
class Channel
{
private:
	std::string _name;
	std::string _topic;
	std::string _key;  // "" = no key (+k off)
	bool _inviteOnly;  // +i
	bool _topicOpOnly; // +t
	int _userLimit;	   // 0 = no limit (+l off)
	std::vector<ClientData *> _members;
	std::vector<ClientData *> _operators;
	std::vector<ClientData *> _invited; // people allowed in even with +i

public:
	Channel(const std::string &name);
	~Channel();

	// membership
	bool isMember(ClientData *c) const;
	bool isOperator(ClientData *c) const;
	bool isInvited(ClientData *c) const;
	bool isEmpty() const;
	size_t memberCount() const;
	void addMember(ClientData *c);
	void removeMember(ClientData *c); // also drops op + invite
	void addOperator(ClientData *c);
	void removeOperator(ClientData *c);
	void addInvite(ClientData *c);

	// send msg to everyone in the channel except 'skip' (NULL = nobody skipped)
	void broadcast(const std::string &msg, ClientData *skip);

	// "@alice bob carol" list for the 353 reply
	std::string namesList() const;
	// "+itk" string for the 324 reply
	std::string modeString() const;

	// getters / setters
	const std::string &getName() const;
	const std::vector<ClientData *> &getMembers() const; // read only, to loop over them
	const std::string &getTopic() const;
	const std::string &getKey() const;
	bool isInviteOnly() const;
	bool isTopicOpOnly() const;
	int getUserLimit() const;
	void setTopic(const std::string &t);
	void setKey(const std::string &k);
	void setInviteOnly(bool on);
	void setTopicOpOnly(bool on);
	void setUserLimit(int n);
};

#endif
