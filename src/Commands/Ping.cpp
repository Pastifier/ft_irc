#include "Commands.hpp"
#include "Server.hpp"

void Ping::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->enqueueMessage(":server 451 :You have not registered");
		return;
	}
	if (params.empty()) {
		client->enqueueMessage(":server 409 " + client->getNickName() + " :No origin specified");
		return;
	}
	std::istringstream iss(params);
	std::string origin;
	iss >> origin;
	std::string pongResponse = ":" + server->getName() + " PONG " + server->getName() + " :" + origin;
	client->enqueueMessage(pongResponse);
}