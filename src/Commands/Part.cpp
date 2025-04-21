
#include "Commands.hpp"
#include "Server.hpp"

void Part::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->sendMessage(":server 451 :You have not registered");
		return;
	}
	std::istringstream iss(params);
	std::string channelList, partMessage;
	if (!(iss >> channelList)) {
		client->sendMessage(":Server 461 " + client->getNickName() + " PART :Not enough parameters\r\n");
		return;
	}
	std::getline(iss >> std::ws, partMessage);
	if (!partMessage.empty() && partMessage[0] == ':') {
		partMessage = partMessage.substr(1);
	}
	if (partMessage.empty()) {
		partMessage = client->getNickName();
	}
	std::vector<std::string> channels;
	std::stringstream ss(channelList);
	std::string channel;
	while (std::getline(ss, channel, ',')) {
		//removes all leading spaces from the channel string.
		channel.erase(0, channel.find_first_not_of(" "));
		channel.erase(channel.find_last_not_of(" ") + 1);
		if (channel.empty() || channel[0] != '#') {
			client->sendMessage(":Server 403 " + client->getNickName() + " " + channel + " :No such channel\r\n");
			continue;
		}
		Channels *targetChannel = server->findChannels(channel);
		if (!targetChannel) {
			client->sendMessage(":server 403 " + client->getNickName() + " " + channel + " :No such channel\r\n");
			continue;
		}
		if (!targetChannel->hasClient(client)) {
			client->sendMessage(":Server 442 " + client->getNickName() + " " + channel + " :You're not on that channel\r\n");
			continue;
		}
		std::string partNotification = ":" + client->getNickName() +
										"!" + client->getUsername() +
										"@" + client->getHostname() +
										" PART " + channel +
										" :"+ partMessage;
		server->sendToChannel(targetChannel, partNotification, NULL);
		targetChannel->removeClient(client);
		client->leaveChannel(targetChannel);
	}
}

/**When a user PARTs a channel:
The user is immediately removed from the channel's user list
A PART message is broadcast to all remaining channel members
The message includes the user's full identifier (nickname!username@hostname)
Optionally includes a part message explaining why they're leaving*/