#include "Commands.hpp"
#include "Server.hpp"

void Topic::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->enqueueMessage(":server 451 " + client->getNickName() + " :You have not registered");
		return;
	}
	std::istringstream iss(params);
	std::string channelName, topicText;
	if (!(iss >> channelName)) {
		client->enqueueMessage(":server 461 " + client->getNickName() + " TOPIC :Not enough parameters");
		return;
	}
	Channels *channel = server->findChannels(channelName);
	if (!channel) {
		client->enqueueMessage(":server 403 " + client->getNickName() + " " + channelName + " :No such channel");
		return;
	}
	if (!client->isInChannel(channel)) {
		client->enqueueMessage(":server 442 " + client->getNickName() + " " + channelName + " :You're not on that channel");
		return;
	}
	size_t topicPos = params.find_first_of(' ');
	if (topicPos != std::string::npos && topicPos + 1 < params.length()) {
		topicText = params.substr(topicPos + 1);
		if (topicText[0] == ':')
			topicText = topicText.substr(1);
		if (channel->isTopicRestricted() && !channel->isOperator(client)) {
			client->enqueueMessage(":server 482 " + client->getNickName() + " " + channelName + " :You're not channel operator");
			return;
		}
		channel->setTopic(topicText);
		std::string topicMessage = ":" + client->getNickName() + "!" + client->getUsername() + "@" +
				client->getHostname() + " TOPIC " + channelName + " :" + topicText;
		std::vector<Client *> channelClients = channel->getClients();
		for (size_t i = 0; i < channelClients.size(); ++i)
			channelClients[i]->enqueueMessage(topicMessage);
	} else {
		if (channel->getTopic().empty()) {
			client->enqueueMessage(":server 331 " + client->getNickName() + " " + channelName + " :No topic is set");
		} else {
			client->enqueueMessage(":server 332 " + client->getNickName() + " " + channelName + " :" + channel->getTopic());
		}
	}
}