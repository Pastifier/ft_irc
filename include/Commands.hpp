#pragma once

#include <string>
#include <iostream>
#include "Server.hpp"
#include "Client.hpp"

class Server;
class Client;

class Commands {
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

