#include "Commands.hpp"
#include "Server.hpp"
#include "IrcBot.hpp"

void CreateBot::execute(Server *server, Client *client, const std::string& params) {
	std::istringstream iss(params);
	std::string botName;

	if (!(iss >> botName)) {
		client->sendMessage(":Server 461 * CREATEBOT :Not enough parameters\r\n");
		client->sendMessage(":Server 461 * CREATEBOT :Usage: CREATEBOT <botname>\r\n");
		return;
	}
	//Check if bot already exists
	if (server->findBot(botName) != NULL) {
		client->sendMessage(":Server 433 * " + botName + " :Bot name already in use\r\n");
		return;
	}
	IrcBot *bot = server->createBot(botName, client->getNickName());
	if (bot == NULL) {
		client->sendMessage(":Server 451 * :Failed to create bot\r\n");
		return;
	}
	client->sendMessage(":server 001" + client->getNickName() + " :Bot " + botName + " created successfully\r\n");
 }