#include "Commands.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channels.hpp"

void Notice::execute(Server *server, Client *client, const std::string& params) {
    if (!client->isRegistered())
        return;

    std::istringstream iss(params);
    std::string target;
    if (!(iss >> target))
        return;

    std::string message;
    std::getline(iss >> std::ws, message);
    if (message.empty() || message[0] != ':')
        return;
    message = message.substr(1); // Remove the leading ':'

    if (target[0] == '#' || target[0] == '&') {
        // Channel notice
        Channels* channel = server->findChannels(target);
        if (!channel || !channel->hasClient(client))
            return;

        // Format and send the notice to all channel members except the sender
        std::string formattedMsg = ":" + client->getNickName() + "!" + client->getUsername() + "@" + 
                                  client->getHostname() + " NOTICE " + target + " :" + message;
        server->sendToChannel(channel, formattedMsg, client);
    }
    else {
        // Private notice to a user
        Client* targetClient = server->findClientByNickname(target);
        if (!targetClient)
            return;

        std::string formattedMsg = ":" + client->getNickName() + "!" + client->getUsername() + "@" + 
                                  client->getHostname() + " NOTICE " + target + " :" + message;
        targetClient->sendMessage(formattedMsg);
    }
}