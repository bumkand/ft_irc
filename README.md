# ft_irc 💬
*This project has been created as part of the 42 curriculum by jaandras, jlager and ksevciko.*

## Description
ft_irc is our own IRC (Internet Relay Chat) server, written in C++98. You connect to it with a real IRC client (our reference client is **irssi**) and use it like any official IRC server: register with a password and a nickname, send private messages, and talk in group channels.

The goal of the project is to understand how a network protocol works from the server side: handling many clients at the same time with non-blocking sockets and a single `poll()`, rebuilding messages that arrive in pieces, and following the IRC rules (RFC 1459 / 2812) closely enough that an existing client works with our server without errors.

What the server can do:
- handle multiple clients at once, without forking and without blocking
- authenticate clients with the server password, set nickname and username
- send and receive private messages between users
- channels: every message sent to a channel is forwarded to all other members
- operators and regular users, with the operator commands `KICK`, `INVITE`, `TOPIC` and `MODE` (`i`, `t`, `k`, `o`, `l`)

## Instructions

### Installation and Compilation
```bash
git clone https://github.com/bumkand/ft_irc.git ft_irc
cd ft_irc
make
```

**Available commands:**
- `make` - Compile the project
- `make clean` - Remove object files
- `make fclean` - Remove objects and executable
- `make re` - Recompile everything if there were any changes to the code (especially header files *.hpp)

### Execution
```bash
./ircserv <port> <password>
```

**...for example:**
```bash
./ircserv 6667 secret
```

### Connecting
With **irssi** (reference client):
```
irssi
/connect 127.0.0.1 6667 secret alice
/join #test
```

With **nc** (`-C` sends `\r\n` line endings, like a real client):
```
nc -C 127.0.0.1 6667
PASS secret
NICK alice
USER alice 0 * :Alice
JOIN #test
PRIVMSG #test :hello everyone
```

## Resources

- [RFC 1459 - Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459) - The original IRC protocol: message format, commands, numeric replies
- [RFC 2812 - IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812) - Updated version of the client protocol, used for command details and error codes
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/) - Readable, up-to-date description of how real servers behave (JOIN/MODE/numerics)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - Sockets, `bind`, `listen`, `accept`, `poll`
- [poll(2) man page](https://man7.org/linux/man-pages/man2/poll.2.html) - How `poll()` and `POLLIN` / `POLLOUT` work
- [irssi documentation](https://irssi.org/documentation/) - Our reference client, used for testing

### AI Usage
- Channels and operator commands (jlager): Claude was used to explain IRC behaviour and numeric replies from the RFCs, to help design and write the channel data structures and the channel commands (JOIN, PART, KICK, INVITE, TOPIC, MODE, WHO), to review the code for bugs and edge cases, to generate edge-case tests, and to fix input validation for `MODE +k` / `+l`.
- Server and connections (jaandras): [how AI was used]
- Parsing, registration and private messages (ksevciko): [how AI was used]

**Note:** Every part generated or suggested by AI was reviewed, tested and understood by the person responsible for it, who can explain every line.

## Team and responsibilities
- **jaandras** - server socket, `poll()` loop, accepting / disconnecting clients, input and output buffers
- **ksevciko** - message parsing, command dispatch, registration (`PASS`, `NICK`, `USER`, `CAP`), `PING`, private messages, `QUIT`
- **jlager** - channels and operator commands: `JOIN`, `PART`, channel messages, `KICK`, `INVITE`, `TOPIC`, `MODE`, `WHO`

## Supported commands

| Command | Usage | Notes |
|---|---|---|
| `PASS` | `PASS <password>` | must match the server password |
| `NICK` | `NICK <nick>` | max 9 chars, case-insensitive, change is shown to everyone sharing a channel |
| `USER` | `USER <user> 0 * :<realname>` | registration is done after PASS + NICK + USER |
| `CAP` | `CAP LS` | answered with an empty list, so clients like irssi can connect |
| `PING` | `PING <token>` | answered with `PONG` |
| `PRIVMSG` / `NOTICE` | `PRIVMSG <nick\|#chan>[,...] :<text>` | `NOTICE` never sends error replies |
| `JOIN` | `JOIN #a[,#b] [key[,key]]` | first person in creates the channel and becomes operator |
| `PART` | `PART #a[,#b] [:reason]` | empty channels are deleted |
| `TOPIC` | `TOPIC #chan [:new topic]` | without text it shows the topic; with `+t` only operators can change it |
| `KICK` | `KICK #chan <nick> [:reason]` | operators only |
| `INVITE` | `INVITE <nick> #chan` | with `+i` only operators can invite |
| `MODE` | `MODE #chan [+/-modes] [args]` | without modes it shows the current ones |
| `WHO` | `WHO #chan` or `WHO <nick>` | irssi sends it after every join |
| `QUIT` | `QUIT [:message]` | everyone sharing a channel sees it once |

### Channel modes

| Mode | Meaning | Argument |
|---|---|---|
| `i` | invite-only channel | - |
| `t` | only operators can change the topic | - |
| `k` | channel key (password) | `+k <key>` (not empty, no `,`), `-k` |
| `o` | give / take operator privilege | `<nick>` for both `+o` and `-o` |
| `l` | user limit | `+l <number>` (1-9999), `-l` |

Several modes can be combined in one command, e.g. `MODE #test +itk-l secret`. Everyone in the channel gets one line with only the changes that really happened.

## Technical choices
- **One `poll()` for everything:** the listening socket and all clients (max 100) are in one `pollfd` array, all sockets are non-blocking.
- **Partial data:** received bytes are added to the client's input buffer, and a command is only handled once a full `\r\n`-terminated line is there (so `com^Dman^Dd` works).
- **Output buffers:** replies are never sent directly. They are added to the client's output buffer and sent only when `poll()` reports `POLLOUT`.
- **Channels:** a `ChannelManager` owns all channels in a `std::map`, keyed by the lowercase name (`#Test` and `#test` are the same channel; the creator's spelling is kept for display). Each `Channel` keeps its members, operators and invited users as pointers to the clients, which live in a fixed array in the server, so the pointers stay valid.
- **Disconnects:** on `QUIT` or a lost connection the client is removed from every channel and invite list *before* its slot is reused, so a new client never inherits someone else's channels or operator rights.
- **Case-insensitive names:** nicks and channel names are compared with RFC 1459 casemapping (`[]\^` equal `{}|~`).
- **MODE decisions:** invalid `+k` (empty or with `,`) and `+l` (not a number, `0`, more than 4 digits) are ignored. An invite only lets you past `+i`; the key and the user limit still apply.
