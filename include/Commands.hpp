#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <vector>
#include <fstream>
#include <map>

struct FileTransfer {
	std::string filename;
	std::string sender;
	std::string recipient;
	std::vector<char> data;
	bool completed;
};

class Server;
class Client;
class Channels;

class Commands {
public:
	virtual ~Commands() {};
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
	void handleUserMode(Server* server, Client* client, const std::string& modeString);
	void handleChannelMode(Server* server, Client* client, Channels* channel, const std::string& modeString, const std::string& modeParams);
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

class Help: public Commands {
private:
	std::string _registrationHelp;
public:
	Help() {
		_registrationHelp = 
			"Register using below commands:\n"
			"PASS [Server Password]\n"
			"NICK [your nickname]\n"
			"[USER sbartoul * *:Suraj Bartoula] or [USER sbartoul 0 *:Suraj Bartoula]\n";
	}
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Bot: public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class CreateBot:public Commands {
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Dcc: public Commands {
private:
	std::map<std::string, FileTransfer> transfers;
	void sendData(Client *clientSender, Client *clientReceiver, const std::string& filename);
	void receiveData(Client *clientSender, Client *clientReceiver, const std::string& filename);
public:
	virtual void execute(Server *server, Client *client, const std::string& params);
};

class Cap: public Commands {
private:
	std::vector<std::string> _supportedCaps;
	void handleCapList(Server *server, Client *client);
	void handleCapRequest(Server *server, Client *client, const std::string& capsRequested);
	void handleCapEnd(Server *server, Client *client);
	void handleCapClear(Server *server, Client *client);
public:
	Cap() {
		_supportedCaps.push_back("multi-prefix");
		_supportedCaps.push_back("away-notify");
		_supportedCaps.push_back("account-notify");
	}

	virtual void execute(Server *server, Client *client, const std::string& params);
};