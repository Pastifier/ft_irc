
#include "Commands.hpp"
#include "Server.hpp"

void Kick::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->enqueueMessage(":Server 451 * KICK :You have not registered\r\n");
		return;
	}
	std::istringstream iss(params);
	std::string channelName, targetNickname, kickReason;
	if (!(iss >> channelName)) {
		client->enqueueMessage(":Server 461 " + client->getNickName() + " KICK :Not enough parameters\r\n");
		return;
	}
	if (!(iss >> targetNickname)) {
		client->enqueueMessage(":Server 461 " + client->getNickName() + " KICK :Not enough parameters\r\n");
		return;
	}
	std::getline(iss >> std::ws, kickReason);
	if (!kickReason.empty() && kickReason[0] == ':') {
		kickReason = kickReason.substr(1);
	}
	if (kickReason.empty()) {
		kickReason = client->getNickName();
	}
	Channels *targetChannel = server->findChannels(channelName);
	if (!targetChannel) {
		client->enqueueMessage(":Server 403 " + client->getNickName() + " " + channelName + " :No such channel\r\n");
		return;
	}
	Client *targetClient = server->findClientByNickname(targetNickname);
	if (!targetClient) {
		client->enqueueMessage(":Server 401 " + client->getNickName() + " " + targetNickname + " :No such nick/channel\r\n");
		return;
	}
	if (!(targetChannel->hasClient(client))) {
		client->enqueueMessage(":Server 442 " + client->getNickName() + " " + channelName + " :You're not on that channel\r\n");
		return;
	}
	if (!(targetChannel->hasClient(targetClient))) {
		client->enqueueMessage(":Server 441 " + client->getNickName() + " " + targetNickname + " " + channelName + " :They are not on that channel\r\n");
		return;
	}
	if (!(targetChannel->isOperator(client))) {
		client->enqueueMessage(":Server 482 " + client->getNickName() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}
	std::string kickNotification = ":" + client->getNickName() +
									"!" + client->getUsername() +
									"@" + client->getHostname() +
									" KICK " + channelName +
									" " + targetNickname + 
									" :" + kickReason;
	server->sendToChannel(targetChannel, kickNotification, NULL);
	targetChannel->removeClient(targetClient);
	targetClient->leaveChannel(targetChannel);
}

/**This implementation of the KICK command follows IRC protocol specifications:

Validates client registration
Parses channel and target nickname
Supports optional kick reason
Performs multiple checks:

Channel existence
Target client existence
Kicking client's channel membership
Target client's channel membership
Operator privileges*/