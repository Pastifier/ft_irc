#include "Commands.hpp"
#include "Server.hpp"

void Invite::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->enqueueMessage(":server 451 " + client->getNickName() + " :You have not registered");
		return;
	}
	std::istringstream iss(params);
	std::string targetNick, channelName;
	if (!(iss >> targetNick >> channelName)) {
		client->enqueueMessage(":server 461 " + client->getNickName() + " INVITE :Not enough parameters");
		return;
	}
	Client *targetClient = server->findClientByNickname(targetNick);
	if (!targetClient) {
		client->enqueueMessage(":server 401 " + client->getNickName() + " " + targetNick + " :No such nick/channel");
		return;
	}
	Channels *channel = server->findChannels(channelName);
	if (!channel) {
		client->enqueueMessage(":server 442 " + client->getNickName() + " " + channelName + " :You're not on that channel");
		return;
	}
	if (!client->isInChannel(channel)) {
		client->enqueueMessage(":server 442 " + client->getNickName() + " " + channelName + " :You're not on that channel");
		return;
	}
	if (channel->isInviteOnly() && !channel->isOperator(client)) {
		client->enqueueMessage(":server 482 " + client->getNickName() + " " + channelName + " :You're not channel operator");
		return;
	}
	if (targetClient->isInChannel(channel)) {
		client->enqueueMessage(":server 443 " + client->getNickName() + " " + targetNick + " " + channelName + " :is already on channel");
		return;
	}
	//send invitation to target user
	targetClient->enqueueMessage(":" + client->getNickName() + "!" + client->getUsername() + "@" + client->getHostname() + 
		" INVITE " + targetNick + " :" + channelName);
	//confirm invitation to inviter
	client->enqueueMessage(":server 341 " + client->getNickName() + " " + targetNick + " " + channelName);
	if (channel->isInviteOnly()) {
		channel->addInvited(targetClient);
		std::vector<Client *> operators = channel->getOperators();
		for (size_t i = 0; i < operators.size(); ++i) {
			if (operators[i] != client) {
				operators[i]->enqueueMessage(":server NOTICE @" + channelName + " :" +
					client->getNickName() + " invited " + targetNick + " to the channel");
			}
		}
	}
}