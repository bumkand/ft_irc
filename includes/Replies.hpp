
#ifndef REPLIES_HPP
#define REPLIES_HPP

#include <string>

// all numeric replies my channel commands need, in one place
// format is always ":<server> <code> <nick> <stuff>\r\n"
// the client uses the code to know what happened, and shows the text after ':'

#define SERVER_NAME "ircserv"

// builds one numeric line. inline = body can live in a header,
// even if many .cpp files include it (no "multiple definition" error)
inline std::string numeric(const std::string &code,
							const std::string &nick,
							const std::string &rest)
{
	return (":" SERVER_NAME " " + code + " " + nick + " " + rest + "\r\n");
}

// nick = who gets the reply, chan / target = std::string

// standardized codes:
// 001–099: connection and registration, e.g. 001 "Welcome to the network"
// 200–399 : normal replies to commands
// 400–599 : errors

// good replies = 3xx
#define RPL_CHANNELMODEIS(nick, chan, modes) numeric("324", nick, chan + " " + modes)
#define RPL_NOTOPIC(nick, chan) numeric("331", nick, chan + " :No topic is set")
#define RPL_TOPIC(nick, chan, topic) numeric("332", nick, chan + " :" + topic)
#define RPL_INVITING(nick, target, chan) numeric("341", nick, target + " " + chan)
#define RPL_NAMREPLY(nick, chan, names) numeric("353", nick, "= " + chan + " :" + names)
#define RPL_ENDOFNAMES(nick, chan) numeric("366", nick, chan + " :End of /NAMES list")

// errors = 4xx
#define ERR_NOSUCHNICK(nick, target) numeric("401", nick, target + " :No such nick/channel")
#define ERR_NOSUCHCHANNEL(nick, chan) numeric("403", nick, chan + " :No such channel")
#define ERR_NORECIPIENT(nick, cmd) numeric("411", nick, ":No recipient given (" + std::string(cmd) + ")")
#define ERR_NOTEXTTOSEND(nick) numeric("412", nick, ":No text to send")
#define ERR_CANNOTSENDTOCHAN(nick, chan) numeric("404", nick, chan + " :Cannot send to channel")
#define ERR_USERNOTINCHANNEL(nick, target, chan) numeric("441", nick, target + " " + chan + " :They aren't on that channel")
#define ERR_NOTONCHANNEL(nick, chan) numeric("442", nick, chan + " :You're not on that channel")
#define ERR_USERONCHANNEL(nick, target, chan) numeric("443", nick, target + " " + chan + " :is already on channel")
#define ERR_NOTREGISTERED(nick) numeric("451", nick, ":You have not registered")
#define ERR_NEEDMOREPARAMS(nick, cmd) numeric("461", nick, std::string(cmd) + " :Not enough parameters")
#define ERR_ALREADYREGISTRED(nick) numeric("462", nick, ":You may not reregister")
#define ERR_PASSWDMISMATCH(nick) numeric("464", nick, ":Password incorrect")
#define ERR_CHANNELISFULL(nick, chan) numeric("471", nick, chan + " :Cannot join channel (+l)")
#define ERR_UNKNOWNMODE(nick, c) numeric("472", nick, std::string(1, c) + " :is unknown mode char to me")
#define ERR_INVITEONLYCHAN(nick, chan) numeric("473", nick, chan + " :Cannot join channel (+i)")
#define ERR_BADCHANNELKEY(nick, chan) numeric("475", nick, chan + " :Cannot join channel (+k)")
#define ERR_CHANOPRIVSNEEDED(nick, chan) numeric("482", nick, chan + " :You're not channel operator")

// text is purpousfully formated " :" ("[space][dvojtecka]")
// the space borders eo previous text and doesnt mess with it than

#endif
