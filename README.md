# ft_irc

## IRC Server written in C++98.

![alt text](irc.png)

Steps to run the program

Step 1
- Run Make
Step 2
* Run the executable as follows:
  ./ircserv port password
Step 3
+ Connect with server using IRC client or nc/telnet
  eg nc ipaddress port followed by (pass password, nick nickname, user username 0 * :realname).*

### Below are the constrains to follow as per the ft_irc 42 rule.
- The server must be capable of handling multiple clients at the same time and never hang.
- Forking is not allowed. All I/O operations must be non-blocking.
- Only 1 poll() (or equivalent) can be used for handling all these operations (read, write, but also listen, and so forth).
- Several IRC clients exist. You have to choose one of them as a reference. Your reference client will be used during the evaluation process.
- Your reference client must be able to connect to your server without encountering any error.
- Communication between client and server has to be done via TCP/IP (v4 or v6).
- Using your reference client with your server must be similar to using it with any official IRC server. However, you only have to implement the following features:
		* You must be able to authenticate, set a nickname, a username, join a channel, send and receive private messages using your reference client.
		* All the messages sent from one client to a channel have to be forwarded to every other client that joined the channel.
		* You must have operators and regular users.
		* Then, you have to implement the commands that are specific to channel
operators:
* KICK - Eject a client from the channel
* INVITE - Invite a client to a channel
* TOPIC - Change or view the channel topic
* MODE - Change the channel’s mode:
- i: Set/remove Invite-only channel
- t: Set/remove the restrictions of the TOPIC command to channel operators
- k: Set/remove the channel key (password)
- o: Give/take channel operator privilege
- l: Set/remove the user limit to channel

you are allowed to use fcntl() only as follows:
+ fcntl(fd, F_SETFL, O_NONBLOCK);
+ Any other flag is forbidden.