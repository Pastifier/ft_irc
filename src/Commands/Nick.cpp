#include "Commands.hpp"
#include "Server.hpp"

bool Nick::isValidNickname(const std::string& nickname) {
	if (nickname.empty() || nickname.length() > 9) {
		return false;
	}
	if (!isalpha(nickname[0]) && nickname[0] != '_') {
		return false;
	}
	for (size_t i = 1; i < nickname.size(); ++i) {
		char c = nickname[i];
		if (!isalnum(c) && c != '_' && c != '-') {
			return false;
		}
	}
	return true;
}

void Nick::execute(Server *server, Client *client, const std::string& params) {
	if (params.empty()) {
		client->enqueueMessage(":server 431 " + client->getNickName() + " :No nickname given");
		return;
	}
	std::string nickname = params;
	if (!isValidNickname(nickname)) {
		client->enqueueMessage(":server 432 " + client->getNickName() + " " + nickname + " :Nickname contains invalid characters.");
		return;
	}
	if (!server->isNicknameAvailable(nickname)) {
		client->enqueueMessage(":server 433 " + client->getNickName() + " " + nickname + " :Nickname is already in use");
		return;
	}
	std::string oldNickname = client->getNickName();
	client->setNickName(nickname);
	if (client->isRegistered()) {
		std::string notification = ":" + oldNickname + "!" + client->getUsername() + "@" + client->getHostname()
			+ " NICK :" + nickname;
		std::vector<Channels *> clientChannels = client->getChannels();
		for (size_t i = 0; i < clientChannels.size(); ++i) {
			server->sendToChannel(clientChannels[i], notification, NULL);
		}
		client->enqueueMessage(notification);
	}
	if (!client->isRegistered() && !client->getUsername().empty()) {
		client->setRegistered(true);
		std::stringstream ss;
		ss << ":server 001 " << nickname << " :Welcome to the IRC Network " << nickname << "!"
			<< client->getUsername() << "@" << client->getHostname();
		client->enqueueMessage(ss.str());

		ss.str("");
		ss << ":server 002 " << nickname << " :Your host is server, running version 1.0";
		client->enqueueMessage(ss.str());

		ss.str("");
		ss << ":server 003 " << nickname << " :This server was created " << __DATE__ << " " << __TIME__;
		client->enqueueMessage(ss.str());

		ss.str("");
		ss << ":server 004 " << nickname << " :server 1.0";
		client->enqueueMessage(ss.str());
	}
}