#include <iostream>
#include <vector>
#include <string>
#include "ClientData.hpp" // the MOCK client (-I . wins over ../includes)
#include "Channel.hpp"
#include "Replies.hpp"
#include "ChannelManager.hpp"

// ============================ helpers ============================

static int g_fail = 0;

// prints OK / FAIL and counts the fails
static void check(bool ok, const std::string &what)
{
	if (ok)
		std::cout << "  [OK]   " << what << std::endl;
	else
	{
		std::cout << "  [FAIL] " << what << std::endl;
		g_fail++;
	}
}

// true if piece is somewhere inside text
static bool has(const std::string &text, const std::string &piece)
{
	return (text.find(piece) != std::string::npos);
}

// params like kata's parser will give them: params("#test") or params("#test", "key")
static std::vector<std::string> params(const std::string &a, const std::string &b = "")
{
	std::vector<std::string> v;

	v.push_back(a);
	if (!b.empty())
		v.push_back(b);
	return (v);
}

// command sent with nothing after it
static std::vector<std::string> noParams()
{
	return (std::vector<std::string>());
}

// ============================ Channel ============================
// SHOULD:     add members, keep ops, put @ before ops in the names list,
//             send a broadcast to every member except the one we skip
// SHOULD NOT: add the same person twice, send to people outside the channel,
//             keep op / invite after someone leaves
static void testChannel()
{
	Channel chan("#test");
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");

	std::cout << "--- Channel ---" << std::endl;
	chan.addMember(&alice);
	chan.addOperator(&alice);
	chan.addMember(&bob);
	chan.addMember(&bob); // on purpose twice
	check(chan.memberCount() == 2, "bob is added only once");
	check(chan.namesList() == "@alice bob", "names list is '@alice bob'");

	chan.broadcast("hi\r\n", &alice); // skip alice
	check(bob.getOutBuf() == "hi\r\n", "bob gets the broadcast");
	check(alice.getOutBuf().empty(), "alice (skipped) gets nothing");
	check(carol.getOutBuf().empty(), "carol (not in channel) gets nothing");

	chan.removeMember(&alice);
	check(!chan.isMember(&alice) && !chan.isOperator(&alice), "alice left > not member, not op");

	chan.setInviteOnly(true);
	chan.setKey("secret");
	check(chan.modeString() == "+ik", "mode string is '+ik'");
}

// ============================ Replies ============================
// SHOULD:     build ":ircserv <code> <nick> <stuff>\r\n"
// SHOULD NOT: forget the \r\n at the end
static void testReplies()
{
	std::string nick = "alice";
	std::string chan = "#test";

	std::cout << "--- Replies ---" << std::endl;
	check(ERR_NOTONCHANNEL(nick, chan) == ":ircserv 442 alice #test :You're not on that channel\r\n",
		  "442 looks right");
	check(ERR_NEEDMOREPARAMS(nick, "JOIN") == ":ircserv 461 alice JOIN :Not enough parameters\r\n",
		  "461 looks right");
}

// ============================ JOIN ============================
// SHOULD:     create the channel, make the first joiner op,
//             send the JOIN line to everyone (joiner too) + names list to the joiner
// SHOULD NOT: make the 2nd joiner op, let someone join twice,
//             let people in past +i / +k / +l, accept a name without #
static void testJoin()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");
	Channel *ch;

	std::cout << "--- JOIN ---" << std::endl;
	mgr.join(&alice, params("#test"));
	ch = mgr.findChannel("#test");
	check(ch != NULL && ch->isOperator(&alice), "channel created, alice is op");
	check(has(alice.getOutBuf(), ":alice!al@localhost JOIN #test"), "alice sees her own JOIN");
	alice.clearOutBuf();

	mgr.join(&bob, params("#test"));
	check(has(alice.getOutBuf(), ":bob!bo@localhost JOIN #test"), "alice sees bob join");
	check(has(bob.getOutBuf(), ":@alice bob"), "bob gets the names list");
	check(!ch->isOperator(&bob), "bob is NOT op");
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.join(&bob, params("#test"));
	check(bob.getOutBuf().empty() && alice.getOutBuf().empty(), "join twice -> nothing happens");

	mgr.join(&carol, noParams());
	check(has(carol.getOutBuf(), " 461 "), "no params -> 461");
	carol.clearOutBuf();
	mgr.join(&carol, params("test"));
	check(has(carol.getOutBuf(), " 403 "), "name without # -> 403");
	carol.clearOutBuf();

	ch->setInviteOnly(true);
	mgr.join(&carol, params("#test"));
	check(has(carol.getOutBuf(), " 473 ") && !ch->isMember(&carol), "+i and not invited -> 473");
	ch->setInviteOnly(false);
	carol.clearOutBuf();

	ch->setKey("secret");
	mgr.join(&carol, params("#test", "wrong"));
	check(has(carol.getOutBuf(), " 475 ") && !ch->isMember(&carol), "+k wrong key -> 475");
	ch->setKey("");
	carol.clearOutBuf();

	ch->setUserLimit(2); // alice + bob = full
	mgr.join(&carol, params("#test"));
	check(has(carol.getOutBuf(), " 471 ") && !ch->isMember(&carol), "+l full -> 471");
}

// ============================ PART ============================
// SHOULD:     send PART to everyone (the leaver too), remove him,
//             delete the channel when the last one leaves
// SHOULD NOT: let a non-member part (442), accept an unknown channel (403)
static void testPart()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");

	std::cout << "--- PART ---" << std::endl;
	mgr.join(&alice, params("#test"));
	mgr.join(&bob, params("#test"));
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.part(&bob, params("#test", "bye"));
	check(alice.getOutBuf() == ":bob!bo@localhost PART #test :bye\r\n", "alice sees bob leave");
	check(bob.getOutBuf() == ":bob!bo@localhost PART #test :bye\r\n", "bob gets his own PART");
	check(!mgr.findChannel("#test")->isMember(&bob), "bob is not a member anymore");

	mgr.part(&carol, params("#test"));
	check(has(carol.getOutBuf(), " 442 "), "carol not in channel -> 442");
	carol.clearOutBuf();
	mgr.part(&carol, params("#nope"));
	check(has(carol.getOutBuf(), " 403 "), "unknown channel -> 403");

	mgr.part(&alice, params("#test"));
	check(mgr.findChannel("#test") == NULL, "last one left -> channel deleted");
}

// ============================ PRIVMSG ============================
// SHOULD:     send the message to every member except the sender
// SHOULD NOT: echo it back to the sender, reach non-members,
//             let a non-member send to the channel (404),
//             send ANY error back for NOTICE
static void testPrivmsg()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");

	std::cout << "--- PRIVMSG ---" << std::endl;
	mgr.join(&alice, params("#test"));
	mgr.join(&bob, params("#test"));
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.privmsg(&alice, "#test", "hello", false);
	check(bob.getOutBuf() == ":alice!al@localhost PRIVMSG #test :hello\r\n", "bob gets the message");
	check(alice.getOutBuf().empty(), "alice does NOT get her own message");
	check(carol.getOutBuf().empty(), "carol (outside) gets nothing");
	bob.clearOutBuf();

	mgr.privmsg(&carol, "#test", "let me in", false);
	check(has(carol.getOutBuf(), " 404 ") && bob.getOutBuf().empty(), "non-member -> 404, nobody gets it");
	carol.clearOutBuf();

	mgr.privmsg(&alice, "#nope", "hi", false);
	check(has(alice.getOutBuf(), " 403 "), "channel doesn't exist -> 403");
	alice.clearOutBuf();

	// NOTICE: same delivery, but never an error back
	mgr.privmsg(&alice, "#test", "psst", true);
	check(bob.getOutBuf() == ":alice!al@localhost NOTICE #test :psst\r\n", "NOTICE reaches bob as NOTICE");
	bob.clearOutBuf();

	mgr.privmsg(&carol, "#test", "let me in", true);
	check(carol.getOutBuf().empty() && bob.getOutBuf().empty(), "NOTICE from non-member -> silent, no 404");

	mgr.privmsg(&alice, "#nope", "hi", true);
	check(alice.getOutBuf().empty(), "NOTICE to missing channel -> silent, no 403");
}

// ============================ QUIT (removeClient) ============================
// SHOULD:     tell everyone who shares a channel with him (once each),
//             remove him from every channel + invite list, delete empty channels
// SHOULD NOT: send QUIT to the quitter, send it twice to the same person
static void testQuit()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");

	std::cout << "--- QUIT (removeClient) ---" << std::endl;
	mgr.join(&alice, params("#a,#b")); // alice + bob share 2 channels
	mgr.join(&bob, params("#a,#b"));
	mgr.findChannel("#a")->addInvite(&carol); // carol only invited
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.removeClient(&bob, "gone");
	check(alice.getOutBuf() == ":bob!bo@localhost QUIT :gone\r\n", "alice gets QUIT exactly once");
	check(bob.getOutBuf().empty(), "bob himself gets nothing");
	check(!mgr.findChannel("#a")->isMember(&bob), "bob removed from #a");

	mgr.removeClient(&carol, "bye");
	check(!mgr.findChannel("#a")->isInvited(&carol), "carol's invite removed");

	mgr.removeClient(&alice, "bye");
	check(mgr.findChannel("#a") == NULL && mgr.findChannel("#b") == NULL, "empty channels deleted");
}

// params with 3 parts, for KICK #chan nick :reason
static std::vector<std::string> params3(const std::string &a, const std::string &b, const std::string &c)
{
	std::vector<std::string> v;

	v.push_back(a);
	v.push_back(b);
	v.push_back(c);
	return (v);
}

// ============================ TOPIC ============================
// SHOULD:     show 331 / 332, let members set it when -t, let ops set it when +t,
//             send the TOPIC line to everyone (setter too), clear it with empty text
// SHOULD NOT: let a non-op change it with +t (482), let a non-member see / set it (442)
static void testTopic()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");
	Channel *ch;

	std::cout << "--- TOPIC ---" << std::endl;
	mgr.join(&alice, params("#test")); // alice = op
	mgr.join(&bob, params("#test"));
	ch = mgr.findChannel("#test");
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.topic(&bob, params("#test"));
	check(has(bob.getOutBuf(), " 331 "), "no topic yet -> 331");
	bob.clearOutBuf();

	mgr.topic(&bob, params("#test", "hello"));
	check(alice.getOutBuf() == ":bob!bo@localhost TOPIC #test :hello\r\n", "-t: bob (not op) sets it, alice sees it");
	check(bob.getOutBuf() == ":bob!bo@localhost TOPIC #test :hello\r\n", "bob sees his own TOPIC");
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.topic(&bob, params("#test"));
	check(bob.getOutBuf() == ":ircserv 332 bob #test :hello\r\n", "topic set -> 332 with the text");
	bob.clearOutBuf();

	ch->setTopicOpOnly(true);
	mgr.topic(&bob, params("#test", "mine now"));
	check(has(bob.getOutBuf(), " 482 ") && ch->getTopic() == "hello", "+t: bob -> 482, topic unchanged");
	check(alice.getOutBuf().empty(), "+t: alice gets nothing from bob's try");
	bob.clearOutBuf();

	mgr.topic(&alice, params("#test", "rules"));
	check(ch->getTopic() == "rules" && has(bob.getOutBuf(), " TOPIC #test :rules"), "+t: op alice can set it");
	alice.clearOutBuf();
	bob.clearOutBuf();

	std::vector<std::string> clear = params("#test");
	clear.push_back(""); // "TOPIC #test :" -> kata's parser gives an empty last param
	mgr.topic(&alice, clear);
	check(ch->getTopic().empty() && has(bob.getOutBuf(), " TOPIC #test :\r\n"), "empty text clears the topic");

	mgr.topic(&carol, params("#test"));
	check(has(carol.getOutBuf(), " 442 "), "carol not in channel -> 442");
	carol.clearOutBuf();
	mgr.topic(&carol, params("#nope"));
	check(has(carol.getOutBuf(), " 403 "), "unknown channel -> 403");
	carol.clearOutBuf();
	mgr.topic(&carol, noParams());
	check(has(carol.getOutBuf(), " 461 "), "no params -> 461");
}

// ============================ KICK ============================
// SHOULD:     let an op kick, send KICK to everyone (kicked one too), remove him + his op,
//             use the kicker's nick as reason if none, match nicks ignoring case
// SHOULD NOT: let a non-op kick (482), let a non-member kick (442),
//             kick someone not in the channel (441)
static void testKick()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");
	ClientData dave(7, "dave", "da");
	Channel *ch;

	std::cout << "--- KICK ---" << std::endl;
	mgr.join(&alice, params("#test")); // alice = op
	mgr.join(&bob, params("#test"));
	mgr.join(&carol, params("#test"));
	ch = mgr.findChannel("#test");
	alice.clearOutBuf();
	bob.clearOutBuf();
	carol.clearOutBuf();

	mgr.kick(&bob, params("#test", "carol"));
	check(has(bob.getOutBuf(), " 482 ") && ch->isMember(&carol), "bob not op -> 482, carol stays");
	bob.clearOutBuf();

	mgr.kick(&dave, params("#test", "carol"));
	check(has(dave.getOutBuf(), " 442 ") && ch->isMember(&carol), "dave not in channel -> 442");
	dave.clearOutBuf();

	ch->addOperator(&bob); // bob is op now, alice kicks him anyway
	mgr.kick(&alice, params3("#test", "BOB", "spam"));
	std::string line = ":alice!al@localhost KICK #test bob :spam\r\n";
	check(alice.getOutBuf() == line && carol.getOutBuf() == line, "everyone sees the KICK (nick written BOB still works)");
	check(bob.getOutBuf() == line, "bob gets his own KICK");
	check(!ch->isMember(&bob) && !ch->isOperator(&bob), "bob is out + lost op");
	alice.clearOutBuf();
	carol.clearOutBuf();
	bob.clearOutBuf();

	mgr.kick(&alice, params("#test", "bob"));
	check(has(alice.getOutBuf(), " 441 "), "bob not in channel anymore -> 441");
	alice.clearOutBuf();

	mgr.kick(&alice, params("#test", "carol"));
	check(carol.getOutBuf() == ":alice!al@localhost KICK #test carol :alice\r\n", "no reason -> kicker's nick");
	alice.clearOutBuf();

	mgr.kick(&alice, params("#test"));
	check(has(alice.getOutBuf(), " 461 "), "no nick -> 461");
	alice.clearOutBuf();
	mgr.kick(&alice, params("#nope", "carol"));
	check(has(alice.getOutBuf(), " 403 "), "unknown channel -> 403");

	mgr.kick(&alice, params("#test", "alice"));
	check(mgr.findChannel("#test") == NULL, "op kicks himself when alone -> channel deleted");
}

// ============================ INVITE ============================
// SHOULD:     find the target in the whole server, send 341 to the inviter + INVITE to the target,
//             let the invited one JOIN a +i channel, let any member invite when -i
// SHOULD NOT: let a non-op invite when +i (482), let a non-member invite (442),
//             invite someone already inside (443), find empty slots / unknown nicks (401)
static void testInvite()
{
	ChannelManager mgr;
	// like jakub's _data array: everyone on the server, slot 4 is empty (fd -1)
	ClientData people[5] = {ClientData(4, "alice", "al"), ClientData(5, "bob", "bo"),
		ClientData(6, "carol", "ca"), ClientData(7, "dave", "da"), ClientData(-1, "ghost", "gh")};
	ClientData &alice = people[0];
	ClientData &bob = people[1];
	ClientData &carol = people[2];
	ClientData &dave = people[3];
	Channel *ch;

	std::cout << "--- INVITE ---" << std::endl;
	mgr.setClients(people, 5);
	mgr.join(&alice, params("#test")); // alice = op
	mgr.join(&bob, params("#test"));
	ch = mgr.findChannel("#test");
	ch->setInviteOnly(true);
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.invite(&bob, params("carol", "#test"));
	check(has(bob.getOutBuf(), " 482 ") && !ch->isInvited(&carol), "+i: bob not op -> 482");
	bob.clearOutBuf();

	mgr.invite(&alice, params("Carol", "#test"));
	check(alice.getOutBuf() == ":ircserv 341 alice carol #test\r\n", "alice gets 341");
	check(carol.getOutBuf() == ":alice!al@localhost INVITE carol :#test\r\n", "carol gets the INVITE (found by Carol too)");
	check(bob.getOutBuf().empty(), "bob (in channel) gets nothing");
	alice.clearOutBuf();
	carol.clearOutBuf();

	mgr.join(&carol, params("#test"));
	check(ch->isMember(&carol), "carol invited -> can JOIN the +i channel");
	carol.clearOutBuf();
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.invite(&alice, params("bob", "#test"));
	check(has(alice.getOutBuf(), " 443 "), "bob already in -> 443");
	alice.clearOutBuf();

	mgr.invite(&dave, params("bob", "#test"));
	check(has(dave.getOutBuf(), " 442 "), "dave not in channel -> 442");
	dave.clearOutBuf();

	mgr.invite(&alice, params("ghost", "#test"));
	check(has(alice.getOutBuf(), " 401 "), "empty slot -> 401");
	alice.clearOutBuf();
	mgr.invite(&alice, params("nobody", "#test"));
	check(has(alice.getOutBuf(), " 401 "), "unknown nick -> 401");
	alice.clearOutBuf();
	mgr.invite(&alice, params("dave", "#nope"));
	check(has(alice.getOutBuf(), " 403 "), "unknown channel -> 403");
	alice.clearOutBuf();
	mgr.invite(&alice, params("dave"));
	check(has(alice.getOutBuf(), " 461 "), "no channel given -> 461");
	alice.clearOutBuf();

	ch->setInviteOnly(false);
	mgr.invite(&bob, params("dave", "#test"));
	check(has(dave.getOutBuf(), " INVITE dave :#test"), "-i: bob (not op) can invite");

	mgr.part(&carol, params("#test"));
	mgr.join(&carol, params("#test"));
	ch->setInviteOnly(true);
	mgr.part(&carol, params("#test"));
	carol.clearOutBuf();
	mgr.join(&carol, params("#test"));
	check(has(carol.getOutBuf(), " 473 "), "invite is used up after leaving -> 473 again");
}

// params with 4 parts, for MODE #chan +kl key 5
static std::vector<std::string> params4(const std::string &a, const std::string &b,
										const std::string &c, const std::string &d)
{
	std::vector<std::string> v = params3(a, b, c);

	v.push_back(d);
	return (v);
}

// ============================ MODE ============================
// SHOULD:     show modes (324), let ops change i t k l o, take args in order,
//             send ONE line with only the real changes to everyone, ignore user modes
// SHOULD NOT: let non-ops change anything (482), announce things that didn't change,
//             accept +k / +l / +o without an arg (461), accept unknown letters (472)
static void testMode()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");
	Channel *ch;

	std::cout << "--- MODE ---" << std::endl;
	mgr.join(&alice, params("#test")); // alice = op
	mgr.join(&bob, params("#test"));
	ch = mgr.findChannel("#test");
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.mode(&bob, params("#test"));
	check(bob.getOutBuf() == ":ircserv 324 bob #test +\r\n", "no modes yet -> 324 +");
	bob.clearOutBuf();

	mgr.mode(&bob, params("#test", "+i"));
	check(has(bob.getOutBuf(), " 482 ") && !ch->isInviteOnly(), "bob not op -> 482");
	bob.clearOutBuf();

	mgr.mode(&alice, params4("#test", "+itkl", "secret", "5"));
	std::string line = ":alice!al@localhost MODE #test +itkl secret 5\r\n";
	check(alice.getOutBuf() == line && bob.getOutBuf() == line, "+itkl -> one line to everyone");
	check(ch->isInviteOnly() && ch->isTopicOpOnly() && ch->getKey() == "secret"
		&& ch->getUserLimit() == 5, "all 4 really set");
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.mode(&bob, params("#test"));
	check(bob.getOutBuf() == ":ircserv 324 bob #test +itkl secret 5\r\n", "324 shows modes + args");
	bob.clearOutBuf();
	mgr.mode(&carol, params("#test"));
	check(carol.getOutBuf() == ":ircserv 324 carol #test +itkl 5\r\n", "outsider doesn't see the key");
	carol.clearOutBuf();

	mgr.mode(&alice, params("#test", "+i"));
	check(alice.getOutBuf().empty(), "+i again -> nothing announced");

	mgr.mode(&alice, params("#test", "-i+t-kl"));
	check(alice.getOutBuf() == ":alice!al@localhost MODE #test -ikl\r\n", "only real changes, signs grouped");
	check(!ch->isInviteOnly() && ch->getKey().empty() && ch->getUserLimit() == 0, "-i -k -l really off");
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.mode(&alice, params3("#test", "+o", "BOB"));
	check(ch->isOperator(&bob) && bob.getOutBuf() == ":alice!al@localhost MODE #test +o bob\r\n", "+o bob");
	alice.clearOutBuf();
	bob.clearOutBuf();
	mgr.mode(&bob, params3("#test", "-o", "alice"));
	check(!ch->isOperator(&alice) && has(alice.getOutBuf(), " MODE #test -o alice"), "new op bob takes alice's op");
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.mode(&bob, params3("#test", "+o", "carol"));
	check(has(bob.getOutBuf(), " 441 "), "+o someone not in channel -> 441");
	bob.clearOutBuf();

	mgr.mode(&bob, params("#test", "+k"));
	check(has(bob.getOutBuf(), " 461 ") && ch->getKey().empty(), "+k without key -> 461");
	bob.clearOutBuf();
	mgr.mode(&bob, params3("#test", "+l", "abc"));
	check(bob.getOutBuf().empty() && ch->getUserLimit() == 0, "+l abc -> ignored");

	mgr.mode(&bob, params("#test", "+xi"));
	check(has(bob.getOutBuf(), " 472 ") && has(bob.getOutBuf(), " MODE #test +i"), "x -> 472, but i still applied");
	bob.clearOutBuf();
	alice.clearOutBuf();

	mgr.mode(&alice, params("#test", "b"));
	check(has(alice.getOutBuf(), " 368 "), "ban list query (irssi) -> 368, no 482");
	alice.clearOutBuf();
	mgr.mode(&alice, params("alice", "+i"));
	check(alice.getOutBuf().empty(), "user mode (no #) -> silently ignored");
	mgr.mode(&alice, params("#nope"));
	check(has(alice.getOutBuf(), " 403 "), "unknown channel -> 403");
	alice.clearOutBuf();
	mgr.mode(&alice, noParams());
	check(has(alice.getOutBuf(), " 461 "), "no params -> 461");
}

// ============================ sendToShared (NICK) ============================
// SHOULD:     reach everyone who shares at least 1 channel with him, once each
// SHOULD NOT: send it to himself, to people in no shared channel, or twice
static void testShared()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");
	ClientData carol(6, "carol", "ca");
	ClientData dave(7, "dave", "da");

	std::cout << "--- sendToShared (NICK) ---" << std::endl;
	mgr.join(&alice, params("#a,#b")); // alice + bob share 2 channels
	mgr.join(&bob, params("#a,#b"));
	mgr.join(&carol, params("#b"));    // carol only shares #b
	mgr.join(&dave, params("#other")); // dave shares nothing
	alice.clearOutBuf();
	bob.clearOutBuf();
	carol.clearOutBuf();
	dave.clearOutBuf();

	std::string msg = ":alice!al@localhost NICK :alicia\r\n";
	mgr.sendToShared(&alice, msg);
	check(bob.getOutBuf() == msg, "bob gets it once (shares 2 channels)");
	check(carol.getOutBuf() == msg, "carol gets it (shares 1 channel)");
	check(dave.getOutBuf().empty(), "dave gets nothing (no shared channel)");
	check(alice.getOutBuf().empty(), "alice herself gets nothing from this");
	check(mgr.findChannel("#a")->isMember(&alice), "she's still in her channels");
}

// ============================ channel names ignore case ============================
// SHOULD:     treat #Test / #TEST / #test as ONE channel, always show the creator's spelling
// SHOULD NOT: create a 2nd channel, show what the joiner typed, leave an empty channel behind
static void testCase()
{
	ChannelManager mgr;
	ClientData alice(4, "alice", "al");
	ClientData bob(5, "bob", "bo");

	std::cout << "--- channel names ignore case ---" << std::endl;
	mgr.join(&alice, params("#Test"));
	alice.clearOutBuf();
	mgr.join(&bob, params("#TEST"));
	check(mgr.findChannel("#test") != NULL && mgr.findChannel("#test")->isMember(&bob), "#TEST joins the same channel as #Test");
	check(alice.getOutBuf() == ":bob!bo@localhost JOIN #Test\r\n", "JOIN shows #Test, not what bob typed");
	check(has(bob.getOutBuf(), " 353 bob = #Test :@alice bob"), "bob's names list: one channel, both inside");
	alice.clearOutBuf();
	bob.clearOutBuf();

	mgr.privmsg(&bob, "#tEsT", "hi", false);
	check(alice.getOutBuf() == ":bob!bo@localhost PRIVMSG #Test :hi\r\n", "PRIVMSG #tEsT reaches #Test");
	alice.clearOutBuf();

	mgr.part(&bob, params("#test"));
	check(alice.getOutBuf() == ":bob!bo@localhost PART #Test\r\n", "PART #test works, shows #Test");
	mgr.part(&alice, params("#TEST"));
	check(mgr.findChannel("#Test") == NULL, "last one left -> deleted (key was lowercase)");

	mgr.join(&alice, params("#Q"));
	mgr.removeClient(&alice, "bye");
	check(mgr.findChannel("#q") == NULL, "QUIT deletes the empty #Q too");
}

// ============================ WHO ============================
// SHOULD:     one 352 per member (@ for ops) + 315 for WHO #chan, one 352 + 315 for WHO nick,
//             always end with 315 (client waits for it)
// SHOULD NOT: answer for unknown channels / nicks / empty slots (just 315)
static void testWho()
{
	ChannelManager mgr;
	ClientData people[3] = {ClientData(4, "alice", "al"), ClientData(5, "bob", "bo"), ClientData(-1, "ghost", "gh")};
	ClientData &alice = people[0];
	ClientData &bob = people[1];

	std::cout << "--- WHO ---" << std::endl;
	mgr.setClients(people, 3);
	mgr.join(&alice, params("#test")); // alice = op
	mgr.join(&bob, params("#test"));
	bob.clearOutBuf();

	mgr.who(&bob, params("#TEST"));
	check(bob.getOutBuf() == ":ircserv 352 bob #test al localhost ircserv alice H@ :0 alice real\r\n"
		":ircserv 352 bob #test bo localhost ircserv bob H :0 bob real\r\n"
		":ircserv 315 bob #TEST :End of WHO list\r\n", "WHO #chan: 352 per member (@ = op) + 315");
	bob.clearOutBuf();

	mgr.who(&bob, params("Alice"));
	check(bob.getOutBuf() == ":ircserv 352 bob * al localhost ircserv alice H :0 alice real\r\n"
		":ircserv 315 bob Alice :End of WHO list\r\n", "WHO nick: one 352 + 315");
	bob.clearOutBuf();

	mgr.who(&bob, params("#nope"));
	check(bob.getOutBuf() == ":ircserv 315 bob #nope :End of WHO list\r\n", "unknown channel -> only 315");
	bob.clearOutBuf();
	mgr.who(&bob, params("ghost"));
	check(bob.getOutBuf() == ":ircserv 315 bob ghost :End of WHO list\r\n", "empty slot -> only 315");
	bob.clearOutBuf();
	mgr.who(&bob, noParams());
	check(bob.getOutBuf() == ":ircserv 315 bob * :End of WHO list\r\n", "WHO alone -> only 315");
}

// ============================ main ============================

int main()
{
	testChannel();
	testReplies();
	testJoin();
	testPart();
	testPrivmsg();
	testQuit();
	testTopic();
	testKick();
	testInvite();
	testMode();
	testShared();
	testCase();
	testWho();

	if (g_fail == 0)
		std::cout << "ALL OK" << std::endl;
	else
		std::cout << g_fail << " FAILED" << std::endl;
	return (g_fail != 0);
}
