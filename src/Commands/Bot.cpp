#include "Commands.hpp"
#include "Server.hpp"
#include "IrcBot.hpp"

void Bot::execute(Server *server, Client *client, const std::string& params) {
	//Convert client to IrcBot
	IrcBot *bot = dynamic_cast<IrcBot *>(client);
	if (bot) {
		std::istringstream iss(params);
		std::string channel, message;
		if (!(iss >> channel)) {
			client->sendMessage(":Server 461 * BOT :Not enough parameters\r\n");
			return;
		}
		std::getline(iss >> std::ws, message);
		if (!message.empty() && message[0] == ':')
			message = message.substr(1);
		bot->processMessage(server, channel, client->getNickName(), message);
	}
}