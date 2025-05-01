#include "Commands.hpp"
#include "Server.hpp"

void User::execute(Server *server, Client *client, const std::string& params) {
	(void)server;
	if (client->isRegistered()) {
		client->enqueueMessage(":server 462 " + client->getNickName() + " :You may not reregister");
		return;
	}
	std::istringstream iss(params);
	std::string username, hostname, servername, realname;
	if (!(iss >> username >> hostname >> servername)) {
		client->enqueueMessage(":server 461 " + client->getNickName() + " USER :Not enough parameters");
		return;
	}
	//Starts searching for ":" in params, but only after servername ends.
	size_t realnamePos = params.find(":", params.find(servername) + servername.length());
	if (realnamePos == std::string::npos) {
		client->enqueueMessage(":server 461 " + client->getNickName() + " USER :Not enough parameters");
		return;
	}
	realname = params.substr(realnamePos + 1);
	client->setUsername(username);
	if (hostname == "0") {
		std::stringstream mid;
		mid << client->getSocket();
		std::string actuaHostname = "client_" + mid.str();
		client->setHostname(actuaHostname);
	} else {
		client->setHostname(hostname);
	}
	client->setRealname(realname);
	if (!client->getNickName().empty()) {
		client->setRegistered(true);
		std::string welcome = ":server 001 " + client->getNickName() + " :Welcome to the IRC server " +
			client->getNickName() + "!" + client->getUsername() + "@" + client->getHostname();
		client->enqueueMessage(welcome);
		std::string yourHost = ":server 002 " + client->getNickName() + " :Your host is server, running version 1.0";
		client->enqueueMessage(yourHost);
		std::string created = ":server 003 " + client->getNickName() + " :This server was created sometime";
		client->enqueueMessage(created);
		std::string myInfo = ":server 004 " + client->getNickName() + " :server 1.0";
		client->enqueueMessage(myInfo);
	}
}