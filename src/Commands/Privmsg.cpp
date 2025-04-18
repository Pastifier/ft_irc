#include "Commands.hpp"
#include "Server.hpp"

void Privmsg::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->sendMessage(":server 451 " + client->getNickName() + " :You have not registered");
		return;
	}
	std::istringstream iss(params);
	std::string target, messageText;
	if (!(iss >> target)) {
		client->sendMessage(":server 411 " + client->getNickName() + " :No recipient given (PRIVMSG)");
		return;
	}
	size_t msgPos = params.find_first_of(' ');
	if (msgPos != std::string::npos && msgPos + 1 < params.length()) {
		messageText = params.substr(msgPos + 1);
		if (messageText[0] == ':')
			messageText = messageText.substr(1);
	} else {
		client->sendMessage(":server 412 " + client->getNickName() + " :No text to send");
		return;
	}
	if (messageText.empty()) {
		client->sendMessage(":server 412 " + client->getNickName() + " :No text to send");
		return;
	}
	std::string formattedMessage = ":" + client->getNickName() + "!" + client->getUsername() + "@" +
			client->getHostname() + " PRIVMSG " + target + " :" + messageText;
	if (target[0] == '#' || target[0] == '&') {
		Channels *channel = server->findChannels(target);
		if (!channel) {
			client->sendMessage(":server 401 " + client->getNickName() + " " + target + " :No such nick/channel");
			return;
		}
		if (!client->isInChannel(channel)) {
			client->sendMessage(":server 404 " + client->getNickName() + " " + target + " :Cannot send to channel");
			return;
		}
		server->sendToChannel(channel, formattedMessage, client);
	} else {
		Client *targetClient = server->findClientByNickname(target);
		if (!targetClient) {
			client->sendMessage(":server 401 " + client->getNickName() + " " + target + " :No such nick/channel");
			return;
		}
		targetClient->sendMessage(formattedMessage);
	}
}