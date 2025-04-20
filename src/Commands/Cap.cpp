#include "Commands.hpp"
#include "Server.hpp"

void Cap::handleCapList(Server *server, Client *client) {
	std::string capList;
	for (std::vector<std::string>::const_iterator it = _supportedCaps.begin(); it != _supportedCaps.end(); ++it) {
		if (it != _supportedCaps.begin()) {
			capList += " ";
		}
		capList += *it;
	}
	std::string response = ":" + server->getName() + " CAP " + client->getNickName() + " LS :" + capList;
	client->sendMessage(response);
}

void Cap::handleCapRequest(Server *server, Client *client, const std::string& capsRequested) {
	std::vector<std::string> requestedCaps;
	std::vector<std::string> acknowledgedCaps;

	//Parse requested capabilites
	std::string::size_type startPos = 0;
	std::string::size_type spacePos;

	while ((spacePos = capsRequested.find(" ", startPos)) != std::string::npos) {
		std::string cap = capsRequested.substr(startPos, spacePos - startPos);
		requestedCaps.push_back(cap);
		startPos = spacePos + 1;
	}
	//Add the last capability
	if (startPos < capsRequested.length())
		requestedCaps.push_back(capsRequested.substr(startPos));
	for (std::vector<std::string>::const_iterator it = requestedCaps.begin(); it != requestedCaps.end(); ++it) {
		for (std::vector<std::string>::const_iterator capIt = _supportedCaps.begin(); capIt != _supportedCaps.end(); ++capIt) {
			if (*it == *capIt) {
				acknowledgedCaps.push_back(*it);
				break;
			}
		}
	}
	//Build acknowledgement string
	std::string ackList;
	for (std::vector<std::string>::const_iterator it = acknowledgedCaps.begin(); it != acknowledgedCaps.end(); ++it) {
		if (it != acknowledgedCaps.begin())
			ackList += " ";
		ackList += *it;
	}
	std::string responses = ":" + server->getName() + " CAP " + client->getNickName() + " ACK :" + ackList;
	client->sendMessage(responses);
}

void Cap::handleCapEnd(Server *server, Client *client) {
	if (!client->isRegistered() && client->getNickName() != "" && client->getUsername() != "") {
		client->setRegistered(true);
		std::string welcomeMsg = ":" + server->getName() + " 001 " + client->getNickName() + " :Welcome to the Internet Relay Network " +
				client->getNickName() + "!" + client->getUsername() + "@" + client->getHostname() + "\r\n";
		client->sendMessage(welcomeMsg);
	}
}

void Cap::handleCapClear(Server *server, Client *client) {
	std::string response = ":" + server->getName() + " CAP " + client->getNickName() + " ACK :*";
	client->sendMessage(response);
}

void Cap::execute(Server *server, Client *client, const std::string& params) {
	std::string::size_type spacePos = params.find(" ");
	std::string subcommand;
	std::string args;
	if (spacePos == std::string::npos) {
		subcommand = params;
	} else {
		subcommand = params.substr(0, spacePos);
		args = params.substr(spacePos + 1);
	}
	//Convert subcommand to uppercase for case-insensitive matching
	std::string subcommandUpper = subcommand;
	for (std::string::iterator it = subcommandUpper.begin(); it != subcommandUpper.end(); ++it) {
		*it = toupper(*it);
	}
	if (subcommandUpper == "LS" || subcommandUpper == "LIST") {
		handleCapList(server, client);
	} else if (subcommandUpper == "REQ") {
		handleCapRequest(server, client, args);
	} else if (subcommandUpper == "END") {
		handleCapEnd(server, client);
	} else if (subcommandUpper == "CLEAR") {
		handleCapClear(server, client);
	} else {
		std::string response = ":" + server->getName() + " 410 " + client->getNickName() + " :Unknown CAP subcommand: " + subcommand;
		client->sendMessage(response);
	}
}