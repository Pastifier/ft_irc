#include "Commands.hpp"

void Pass::execute(Server *server, Client *client, const std::string& params) {
	//If client is already authenticated, reject
	if (client->isAuthenticated()) {
		client->sendMassage(":server 462 " + client->getNickName() + " :You may not reregister");
	}
	//Check if password is provided
	if (params.empty()) {
		client->sendMassage(":server 461 " + client->getNickName() + " PASS :Not enought parameters");
		return;
	}
	//Extract passwod (may have a prefix ":")
	std::string password = params;
	if (password[0] == ':') {
		password = password.substr(1);
	}
	//Validate password
	if (password == server->getPassword()) {
		client->setAuthenticated(true);
		//No success message is sent for PASS command
	} else {
		client->sendMassage(":server 464 " + client->getNickName() + " :Password incorrect");
	}
}