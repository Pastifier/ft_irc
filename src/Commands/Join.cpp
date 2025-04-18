#include "Commands.hpp"
#include "Server.hpp"

void Join::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->sendMessage(":server 451 " + client->getNickName() + " :You have not registered");
		return;
	}
	std::istringstream iss(params);
	std::string channelParameter, keyParameter;
	if (!(iss >> channelParameter)) {
		client->sendMessage(":server 461 " + client->getNickName() + " Join :Not enough parameters");
		return;
	}
	iss >> keyParameter;
	//Container to handle multiple channels(comma-seperated)
	std::vector<std::string> channelNames;
	std::vector<std::string> keys;
	//parse channel names split by comma
	size_t pos = 0;
	std::string token;
	while ((pos = channelParameter.find(',')) != std::string::npos) {
		token = channelParameter.substr(0, pos);
		channelNames.push_back(token);
		channelParameter.erase(0, pos + 1);
	}
	channelNames.push_back(channelParameter); //add the last channel
	if (!keyParameter.empty()) {
		pos = 0;
		while ((pos = keyParameter.find(',')) != std::string::npos) {
			token = keyParameter.substr(0, pos);
			keys.push_back(token);
			keyParameter.erase(0, pos + 1);
		}
		keys.push_back(keyParameter);
	}
	for (size_t i = 0; i < channelNames.size(); ++i) {
		const std::string& channelName = channelNames[i];
		//check if channel name starts with # or &
		if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&')) {
			client->sendMessage(":server 403 " + client->getNickName() + " " + channelName + " No such channel");
			continue;
		}
		//get the key for this channel if provided.
		std::string providedkey = (i < keys.size()) ? keys[i] : "";
		Channels *channel = server->findChannels(channelName);
		if (!channel) {
			channel = server->createChannels(channelName);
			if (!channel) {
				client->sendMessage(":server 403 " + client->getNickName() + " " + channelName + " :Cannot create channel");
				continue;
			}
			//make the first user a channel operator
			channel->addOperator(client);
		} else {
			if (client->isInChannel(channel)) {
				continue;
			}
			//check user limit
			if (channel->getUserLimit() > 0 && channel->getClientCount() >= channel->getUserLimit()) {
				client->sendMessage(":server 471 " + client->getNickName() + " " + channelName + " :Cannot join channel (+l)");
				continue;
			}
			if (!channel->getPassword().empty()) {
				if (providedkey != channel->getPassword()) {
					client->sendMessage(":server 475 " + client->getNickName() + " " + channelName + " :Cannot join channel (+k)");
					continue;
				}
			}
			if (channel->isInviteOnly() && !channel->isInvited(client) && !channel->isOperator(client)) {
				client->sendMessage(":server 473 " + client->getNickName() + " " + channelName + " :Cannot join channel (+i)");
				continue;
			}
		}
		client->joinChannel(channel);
		channel->addClient(client);
		//if the client was in the invited list, remove them now that they've joined
		if (channel->isInvited(client))
			channel->removeInvited(client);
		//Send JOIN notification to all the users in channel
		std::string joinMessage = ":" + client->getNickName() + "!" + client->getUsername() + "@" + client->getHostname() +
				" JOIN :" + channelName;
		std::vector<Client *> channelClients = channel->getClients();
		for (size_t j = 0; j < channelClients.size(); ++j)
			channelClients[j]->sendMessage(joinMessage);
		if (channel->getTopic().length() > 0)
			client->sendMessage(":server 332 " + client->getNickName() + " " + channelName + " :" + channel->getTopic());
		//send list of users in channel
		std::string namesList;
		for (size_t j = 0; j < channelClients.size(); ++j) {
			Client *channelClient = channelClients[j];
			//add prefix @ for channel operators
			if (channel->isOperator(channelClient))
				namesList += "@";
			namesList += channelClient->getNickName();
			if (j < channelClients.size() - 1)
				namesList += " ";
		}
		client->sendMessage(":server 353 " + client->getNickName() + " = " + channelName + " :" + namesList);
		client->sendMessage(":server 366 " + client->getNickName() + " " + channelName + " :End of /NAMES list");
	}
}