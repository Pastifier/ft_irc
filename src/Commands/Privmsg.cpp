#include "Commands.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channels.hpp"

void Privmsg::execute(Server *server, Client *client, const std::string& params) {
    if (!client->isRegistered()) {
        client->sendMessage(":server 451 * :You have not registered");
        return;
    }

    std::istringstream iss(params);
    std::string target;
    if (!(iss >> target)) {
        client->sendMessage(":server 411 " + client->getNickName() + " :No recipient given (PRIVMSG)");
        return;
    }

    std::string message;
    std::getline(iss >> std::ws, message);
    if (message.empty() || message[0] != ':') {
        client->sendMessage(":server 412 " + client->getNickName() + " :No text to send");
        return;
    }
    message = message.substr(1); // Remove the leading ':'

    if (target[0] == '#' || target[0] == '&') {
        // Channel message
        Channels* channel = server->findChannels(target);
        if (!channel) {
            client->sendMessage(":server 403 " + client->getNickName() + " " + target + " :No such channel");
            return;
        }

        if (!channel->hasClient(client)) {
            client->sendMessage(":server 404 " + client->getNickName() + " " + target + " :Cannot send to channel");
            return;
        }

        // Format and send the message to all channel members except the sender
        std::string formattedMsg = ":" + client->getNickName() + "!" + client->getUsername() + "@" + 
                                  client->getHostname() + " PRIVMSG " + target + " :" + message;
        server->sendToChannel(channel, formattedMsg, client);
    }
    else {
        // Private message to a user
        Client* targetClient = server->findClientByNickname(target);
        if (!targetClient) {
            client->sendMessage(":server 401 " + client->getNickName() + " " + target + " :No such nick/channel");
            return;
        }

        std::string formattedMsg = ":" + client->getNickName() + "!" + client->getUsername() + "@" + 
                                  client->getHostname() + " PRIVMSG " + target + " :" + message;
        targetClient->sendMessage(formattedMsg);
    }
}