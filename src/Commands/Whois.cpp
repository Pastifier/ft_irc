#include "Commands.hpp"
#include "Server.hpp"

void Whois::execute(Server *server, Client *client, const std::string& params) {
	if (params.empty()) {
		client->sendMessage(":server 431 " + client->getNickName() + " :No nickname given");
		return;
	}
	std::string targetNick = params;
	size_t spacePos = params.find(' ');
	if (spacePos != std::string::npos) {
		targetNick = params.substr(0, spacePos);
	}
	Client *targetClient = server->findClientByNickname(targetNick);
	if (!targetClient) {
		client->sendMessage(":server 401 " + client->getNickName() + " " + targetNick + " :No such nickname");
		return;
	}
	//Send basic user info
	client->sendMessage(":server 311 " + client->getNickName() + " " + targetClient->getNickName() + " " +
			targetClient->getUsername() + " " + targetClient->getHostname() + " * :" +
			targetClient->getRealname());
	//Send server info
	client->sendMessage(":server 312 " + client->getNickName() + " " + targetClient->getNickName() +
			" " + server->getName() + " :" + server->getVersion());
	//if the client is operator
	if (targetClient->isOperator()) {
		client->sendMessage(":server 313 " + client->getNickName() + " " + targetClient->getNickName() +
			" :is an IRC operator");
	}
	//what channel user is in
	std::string channelList;
	std::vector<Channels *> userChannels = targetClient->getChannels();
	for (size_t i = 0; i < userChannels.size(); ++i) {
		Channels *channel = userChannels[i];
		if (channel->isOperator(targetClient))
			channelList += "@";
		channelList += channel->getName();
		if (i < userChannels.size() - 1)
			channelList += " ";
	}
	if (!channelList.empty()) {
		client->sendMessage(":server 319 " + client->getNickName() + " " + targetClient->getNickName() +
				" :" + channelList);
	}
	client->sendMessage(":server 318 " + client->getNickName() + " " + targetClient->getNickName() +
			" :End of /WHOIS list");
}