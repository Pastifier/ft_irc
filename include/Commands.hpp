#pragma once

#include <string>
#include <iostream>
#include "Server.hpp"
#include "Client.hpp"
#include "Channels.hpp"

class Server;
class Client;
class Channels;

class Commands {
public:
	virtual ~Commands();
	virtual void execute(Server *server, Client *client, const std::string& params) = 0;
};

class Pass: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Nick: public Commands {
private:
	bool isValidNickname(const std::string& nickname);
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class User: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Join: public Commands {
private:
	bool canClientJoinChannel(Server *server, Client *client, const std::string& params);
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Part: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Privmsg: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Notice: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Quit: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Mode: public Commands {
private:
	void handleChannelMode(Server *server, Client *client, const std::string& target, const std::string& modeFlags);
	void handleUserMode(Server *server, Client *client, const std::string& target, const std::string& modeFlags);
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Topic: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Kick: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Invite: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Ping: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Oper: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Cap: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Help: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};