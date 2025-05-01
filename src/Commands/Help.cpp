#include "Commands.hpp"
#include "Server.hpp"

void Help::execute(Server *server, Client *client, const std::string& params) {
	(void)server;
	(void)params;
	std::istringstream helpstream(_registrationHelp);
	std::string line;
	while (std::getline(helpstream, line)) {
		client->enqueueMessage(line);
	}
}
