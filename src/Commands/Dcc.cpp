#include "Commands.hpp"
#include "Server.hpp"

void Dcc::sendData(Client *clientSender, Client *clientReceiver, const std::string& filename) {
	FileTransfer transfer;
	transfer.filename = filename;
	transfer.sender = clientSender->getNickName();
	transfer.recipient = clientReceiver->getNickName();
	transfer.completed = false;

	std::ifstream file(filename.c_str(), std::ios::binary);
	if (file) {
		//Read file content into the transfer data buffer
		transfer.data = std::vector<char>(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);
		//store transfer in the map
		transfers[filename] = transfer;
		// Conversion
		std::stringstream mid;
		mid << transfer.data.size();
		//notify the receiver about the file offer
		std::string message = ":" + clientSender->getNickName() +
			" NOTICE " + clientReceiver->getNickName() +
			" :DCC SEND " + filename + " " +
			mid.str() + " bytes";
		clientReceiver->sendMessage(message);
		clientSender->sendMessage(":server NOTICE " + clientSender->getNickName() +
			" :DCC SEND offer sent to " + clientReceiver->getNickName());
	} else {
		//file not found, notify sender
		clientSender->sendMessage(":server NOTICE " + clientSender->getNickName() +
			" :Error: File not found - " + filename);
	}
}

void Dcc::receiveData(Client *clientSender, Client *clientReceiver, const std::string& filename) {
	//check if the file transfer exists
	if (transfers.find(filename) == transfers.end()) {
		clientReceiver->sendMessage(":server NOTICE " + clientReceiver->getNickName() +
				" :Error: No such file transfer - " + filename);
		return;
	}
	FileTransfer& transfer = transfers[filename];
	if (transfer.sender != clientSender->getNickName() || 
		transfer.recipient != clientReceiver->getNickName()) {
		clientReceiver->sendMessage(":server NOTICE " + clientReceiver->getNickName() + 
		" :Error: You are not authorized for this transfer");
		return;
	}
	transfer.completed = true;
	//notify receiver
	// Conversion
	std::stringstream mid;
	mid << transfer.data.size();
	std::string message = ":server NOTICE " + clientReceiver->getNickName() +
		" :DCC GET complete - " + filename + "  (" +
		mid.str() + " bytes )";
	clientReceiver->sendMessage(message);
	//notify sender
	clientSender->sendMessage(":server NOTICE " + clientSender->getNickName() +
			" :DCC transfer complete - " + filename);
}

void Dcc::execute(Server *server, Client *client, const std::string& params) {
	//Clean up completed transfers first
	for (std::map<std::string, FileTransfer>::iterator it = transfers.begin(); it != transfers.end(); ++it) {
		if (it->second.completed) {
			std::map<std::string, FileTransfer>::iterator temp = it;
			// temp->second.~FileTransfer();
			transfers.erase(temp);
			// it = transfers.erase(it);
		}
		// else
		// 	++it;
		// ++it;
	}

	//parse DCC command
	std::istringstream iss(params);
	std::string subCommand, targetNick, filename;
	//get the subcommand(SEND or GET)
	iss >> subCommand;
	iss >> targetNick;
	iss >> filename;
	Client *targetClient = server->findClientByNickname(targetNick);
	if (!targetClient) {
		client->sendMessage(":server NOTICE " + client->getNickName() + 
			" :Error: No such nickname - " + targetNick);
		return;
	}
	if (subCommand == "SEND")
		sendData(client, targetClient, filename);
	else if (subCommand == "GET")
		receiveData(targetClient, client, filename);
	else {
		client->sendMessage(":server NOTICE " + client->getNickName() + 
			" :Error: Unkown DCC comand - " + subCommand);
	}
}