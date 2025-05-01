#include "Commands.hpp"
#include "Server.hpp"

void Notice::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered())
		return;
	std::istringstream iss(params);
	std::string target, messageText;
	if (!(iss >> target)) {
		return;
	}
	//Get the message text (everything after the first space)
	size_t msgPos = params.find_first_of(' ');
	if (msgPos != std::string::npos && msgPos + 1 < params.length()) {
		messageText = params.substr(msgPos + 1);
		//if message starts with : strip it
		if (messageText[0] == ':')
			messageText = messageText.substr(1);
	} else {
		//No message provided
		return;
	}
	if (messageText.empty())
		return;
	std::string formattedMessage = ":" + client->getNickName() + "!" + client->getUsername() + "@" +
			client->getHostname() + " NOTICE " + target + " :" + messageText;
	//check if target is channel
	if (target[0] == '#' || target[0] == '&') {
		Channels *channel = server->findChannels(target);
		if (!channel)
			return;
		if (!client->isInChannel(channel))
			return;
		server->sendToChannel(channel, formattedMessage, client);
	} else {
		Client *targetClient = server->findClientByNickname(target);
		if (!targetClient)
			return;
		targetClient->enqueueMessage(formattedMessage);
	}
}