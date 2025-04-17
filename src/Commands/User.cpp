#include "Commands.hpp"
#include "Server.hpp"

void User::execute(Server *server, Client *client, const std::string& params) {
	(void)server;
	if (client->isRegistered()) {
		client->sendMessage(":server 462 " + client->getNickName() + " :You may not reregister");
		return;
	}
	std::istringstream iss(params);
	std::string username, hostname, servername, realname;
	if (!(iss >> username >> hostname >> servername)) {
		client->sendMessage(":server 461 " + client->getNickName() + " USER :Not enough parameters");
		return;
	}
	//Starts searching for ":" in params, but only after servername ends.
	size_t realnamePos = params.find(":", params.find(servername) + servername.length());
	if (realnamePos == std::string::npos) {
		client->sendMessage(":server 461 " + client->getNickName() + " USER :Not enough parameters");
		return;
	}
	realname = params.substr(realnamePos + 1);
	client->setUsername(username);
	if (hostname == "0") {
		std::string actuaHostname = "client_" + std::to_string(client->getSocket());
		client->setHostname(actuaHostname);
	} else {
		client->setHostname(hostname);
	}
	client->setRealname(realname);
	if (!client->getNickName().empty()) {
		client->setRegistered(true);
		std::string welcome = ":server 001 " + client->getNickName() + " :Welcome to the IRC server " +
			client->getNickName() + "!" + client->getUsername() + "@" + client->getHostname();
		client->sendMessage(welcome);
		std::string yourHost = ":server 002 " + client->getNickName() + " :Your host is server, running version 1.0";
		client->sendMessage(yourHost);
		std::string created = ":server 003 " + client->getNickName() + " :This server was created sometime";
		client->sendMessage(created);
		std::string myInfo = ":server 004 " + client->getNickName() + " :server 1.0 o o";
		client->sendMessage(myInfo);
	}
}