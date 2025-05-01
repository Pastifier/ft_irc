#include "Commands.hpp"
#include "Server.hpp"

void Oper::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->enqueueMessage(":server 451 :You have not registered");
		return;
	}
	std::istringstream iss(params);
	std::string username, password;
	if (!(iss >> username >> password)) {
		client->enqueueMessage(":server 461 " + client->getNickName() + " OPER :Not enough parameters");
		return;
	}
	if (username != client->getUsername()) {
		client->enqueueMessage(":server 464 " + client->getNickName() + " :Username incorrect");
		return;
	}
	if (password == server->getPassword()) {
		client->setOperator(true);
		client->enqueueMessage(":server 381 " + client->getNickName() + " :You are now an IRC operator");
		std::string opNotice = ":" + client->getNickName() + "!" + client->getUsername() + "@" +
			client->getHostname() + " MODE " + client->getNickName() + " :+o";
		server->broadcastMessage(opNotice);
	} else {
		client->enqueueMessage(":server 464 " + client->getNickName() + " :Password incorrect");
	}
}