#include "Commands.hpp"
#include "Server.hpp"

void Quit::execute(Server *server, Client *client, const std::string& params) {
	std::string quitMessage = "Quit";
	if (!params.empty()) {
		// If the quit message starts with :, remove the colon
		quitMessage = (params[0] == ':') ? params.substr(1) : params;
	}
	std::string quitNotification = ":" + client->getNickName() +
									"!" + client->getUsername() +
									"@" + client->getHostname() +
									" QUIT : " + quitMessage;
	std::vector<Channels *> channels = client->getChannels();
	for (std::vector<Channels *>::iterator it = channels.begin(); it != channels.end(); ++it) {
		server->sendToChannel(*it, quitNotification, NULL);
		(*it)->removeClient(client);
	}
	server->broadcastMessage(quitNotification);
	int clientSocket = client->getSocket();
	close(clientSocket);
	server->removeClient(client);
}