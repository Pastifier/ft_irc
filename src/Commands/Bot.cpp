#include "Commands.hpp"
#include "Server.hpp"
#include "IrcBot.hpp"

void Bot::execute(Server *server, Client *client, const std::string& params) {
	std::istringstream iss(params);
	std::string botName, channel, message;

	if (!(iss >> botName >> channel)) {
		client->enqueueMessage(":Server 461 * BOT :Not enough parameters\r\n");
		client->enqueueMessage(":Server 461 * BOT :Usage: BOT <botname> <channel> <message>\r\n");
		return;
	}
	std::getline(iss >> std::ws, message);
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);

	//Find the bot by name
	IrcBot *bot = server->findBot(botName);
	if (bot == NULL) {
		client->enqueueMessage(":Server 403 * " + botName + " :Bot not found\r\n");
		return;
	}
	bot->processMessage(server, channel, client->getNickName(), message);
}