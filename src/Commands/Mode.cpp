#include "Commands.hpp"
#include "Server.hpp"

void Mode::execute(Server *server, Client *client, const std::string& params) {
	if (!client->isRegistered()) {
		client->enqueueMessage(":server 451 :You have not registered");
		return;
	}
	std::istringstream iss(params);
	std::string target, modeString, modeParams;
	if (!(iss >> target)) {
		client->enqueueMessage(":server 461 " + client->getNickName() + " MODE :Not enough parameters");
		return;
	}
	iss >> modeString;
	std::string param;
	while (iss >> param) {
		if (!modeParams.empty()) {
			modeParams += " ";
		}
		modeParams += param;
	}
	if (target[0] == '#' || target[0] == '&') {
		Channels *channel = server->findChannels(target);
		if (!channel) {
			client->enqueueMessage(":server 403 " + client->getNickName() + " " + target + " :No such channel");
			return;
		}
		//If command is --> MODE #channel then the response will be :server 324 User123 #channel +itkl
		//below will write +itkl baseon on the channel config.
		if (modeString.empty()) {
			std::string modes = "+";
			if (channel->isInviteOnly())
				modes += "i";
			if (channel->isTopicRestricted())
				modes += "t";
			if (!channel->getPassword().empty())
				modes += "k";
			if (channel->getUserLimit() > 0)
				modes += "l";
			client->enqueueMessage(":server 324 " + client->getNickName() + " " + target + " " + modes);
			return;
		}
		handleChannelMode(server, client, channel, modeString, modeParams);
	} else {
		if (target != client->getNickName()) {
			client->enqueueMessage(":server 502 " + client->getNickName() + " :Cannot change mode for other users");
			//:server 502 User123 :Cannot change mode for other users
			return;
		}
		if (modeString.empty()) {
			std::string modes = "+";
			if (client->isOperator())
				modes += "o";
			client->enqueueMessage(":server 221 " + client->getNickName() + " " + modes); //:server 221 User123 +o

			return;
		}
		handleUserMode(server, client, modeString);
	}
}

/**
 * @brief Processes user mode changes in an IRC server, ensuring that only valid mode changes are applied
 * 
 * @param server 
 * @param client 
 * @param target 
 * @param modeString 
 */
void Mode::handleUserMode(Server *server, Client *client, const std::string& modeString) {
	bool adding = true;
	bool modeChanged = false;

	for (size_t i = 0; i < modeString.length(); ++i) {
		char mode = modeString[i];
		if (mode == '+')
			adding = true;
		else if (mode == '-')
			adding = false;
		else if (mode == 'o') {
			// The 'o' flag can only be removed, not added via MODE
			if (!adding && client->isOperator()) {
				client->setOperator(false);
				modeChanged = true;
			} else if (adding)
				client->enqueueMessage(":server 472 " + client->getNickName() + " o :Cannot set mode +o on self");
		}
	}
	if (modeChanged) {
		std::string modeNotice = ":" + client->getNickName() + "!" + client->getUsername() + "@" +
			client->getHostname() + " MODE " + client->getNickName() + " " + modeString;
		server->broadcastMessage(modeNotice);
	}
}

/**
 * @brief Only channel operators or server operators can modify channel modes and applies the requested changes accordingly
 * @param server 
 * @param client 
 * @param channel 
 * @param modeString 
 * @param modeParams 
 */
void Mode::handleChannelMode(Server *server, Client *client, Channels *channel, const std::string& modeString, const std::string& modeParams) {
	//Neither a channel operator nor a global server operator | handleChannelMode(server, client, channel, "+i", "");
	if (!channel->isOperator(client) && !client->isOperator()) {
		client->enqueueMessage(":server 482 " + client->getNickName() + " " + channel->getName() + " :You're not channel operator");
		return;
	}
	bool adding = true;
	bool modeChanged = false;
	std::string effectiveModes = "";
	std::string effectiveParams = "";
	std::istringstream paramStream(modeParams);
	std::string currentParam;

	for (size_t i = 0; i < modeString.length(); ++i) {
		char mode = modeString[i];
		if (mode == '+') {
			if (!adding) {
				effectiveModes += "+";
				adding = true;
			}
		} else if (mode == '-') {
			if (adding) {
				effectiveModes += "-";
				adding = false;
			}
		} else {
			switch (mode)
			{
			case 'i':
				channel->setInviteOnly(adding);
				effectiveModes += mode;
				modeChanged = true;
				break;
			case 't':
				channel->setTopicRestricted(adding);
				effectiveModes += mode;
				modeChanged = true;
				break;
			case 'k':
				if (adding) {
					if (paramStream >> currentParam) {
						channel->setPassword(currentParam);
						effectiveModes += mode;
						effectiveParams += " " + currentParam;
						modeChanged = true;
					} else {
						client->enqueueMessage(":server 461 " + client->getNickName() + "MODE :Not enough parameters");
					}
				} else { // If the mode is being removed (-k)
					channel->setPassword("");
					effectiveModes += mode;
					modeChanged = true;
				}
				break;
			case 'l':
				if (adding) { //eg MODE #general +l 10
					if (paramStream >> currentParam) {
						char *end;
						unsigned int limit = static_cast<unsigned int>(strtol(currentParam.c_str(), &end, 10));
						if (*end == '\0') {
							channel->setUserLimit(limit);
							effectiveModes += mode;
							effectiveParams += " " + currentParam;
							modeChanged = true;
						} else {
							client->enqueueMessage(":server 461 " + client->getNickName() + " MODE :Invalid user limit");
						}
					} else {
						client->enqueueMessage(":server 461 " + client->getNickName() + "MODE :Not enough parameters");
					}
				} else {
					channel->setUserLimit(0);
					effectiveModes += mode;
					modeChanged = true;
				}
				break;
			case 'o':
				if (paramStream >> currentParam) {
					Client *targetClient = server->findClientByNickname(currentParam);
					if (targetClient && channel->hasClient(targetClient)) {
						if (adding) {
							channel->addOperator(targetClient);
						} else {
							channel->removeOperator(targetClient);
						}
						effectiveModes += mode;
						effectiveParams += " " + currentParam;
						modeChanged = true;
					} else {
						client->enqueueMessage(":server 441 " + client->getNickName() + " " + currentParam + " " +
							channel->getName() + " :They aren't on that channel");
					}
				} else {
					client->enqueueMessage(":server 461 " + client->getNickName() + " MODE :Not enough parameters");
				}
				break;
			default:
				break;
			}
		}
	}
	if (modeChanged) {
		std::string modeMessage = ":" + client->getNickName() + "!" + client->getUsername() + "@" +
		client->getHostname() + " MODE " + channel->getName() + " " + effectiveModes + effectiveParams;
		server->sendToChannel(channel, modeMessage, NULL);
		//eg :NickName!UserName@HostName MODE #channel +l 10
	}
}