#include "Commands.hpp"
#include "Server.hpp"
#include "IrcBot.hpp"

void CreateBot::execute(Server *server, Client *client, const std::string& params) {
	std::istringstream iss(params);
	std::string botName;

	if (!(iss >> botName)) {
		client->enqueueMessage(":Server 461 * CREATEBOT :Not enough parameters\r\n");
		client->enqueueMessage(":Server 461 * CREATEBOT :Usage: CREATEBOT <botname>\r\n");
		return;
	}
	//Check if bot already exists
	if (server->findBot(botName) != NULL) {
		client->enqueueMessage(":Server 433 * " + botName + " :Bot name already in use\r\n");
		return;
	}
	IrcBot *bot = server->createBot(botName, client->getNickName());
	if (bot == NULL) {
		client->enqueueMessage(":Server 451 * :Failed to create bot\r\n");
		return;
	}
	client->enqueueMessage(":server 001" + client->getNickName() + " :Bot " + botName + " created successfully\r\n");
 }