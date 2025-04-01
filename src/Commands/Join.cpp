#include "Commands.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channels.hpp"

bool Join::canClientJoinChannel(Server *server, Client *client, const std::string& channelName) {
    Channels* channel = server->findChannels(channelName);
    if (!channel)
        return true;

    // Check if channel is invite-only
    if (channel->isInviteOnly()) {
        client->sendMessage(":server 473 " + client->getNickName() + " " + channelName + " :Cannot join channel (+i)");
        return false;
    }

    // Check user limit
    if (channel->getUserLimit() > 0 && channel->getClientCount() >= channel->getUserLimit()) {
        client->sendMessage(":server 471 " + client->getNickName() + " " + channelName + " :Cannot join channel (+l)");
        return false;
    }

    return true;
}

void Join::execute(Server *server, Client *client, const std::string& params) {
    if (!client->isRegistered()) {
        client->sendMessage(":server 451 * :You have not registered");
        return;
    }

    if (params.empty()) {
        client->sendMessage(":server 461 " + client->getNickName() + " JOIN :Not enough parameters");
        return;
    }

    std::istringstream iss(params);
    std::string channelList, keyList;
    iss >> channelList;
    iss >> keyList;

    std::istringstream channelStream(channelList), keyStream(keyList);
    std::string channelName, key;

    while (std::getline(channelStream, channelName, ',')) {
        // Check if channel name is valid
        if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&')) {
            client->sendMessage(":server 403 " + client->getNickName() + " " + channelName + " :No such channel");
            continue;
        }

        // Get key if available
        if (!keyList.empty()) {
            if (!std::getline(keyStream, key, ','))
                key = "";
        }

        // Check if client can join
        if (!canClientJoinChannel(server, client, channelName))
            continue;

        // Get or create channel
        Channels* channel = server->findChannels(channelName);
        if (!channel) {
            channel = server->createChannels(channelName);
            if (!channel) {
                client->sendMessage(":server 403 " + client->getNickName() + " " + channelName + " :Cannot create channel");
                continue;
            }
            channel->addOperator(client); // First user becomes operator
        }
        else {
            // Check password if set
            if (!channel->getPassword().empty() && channel->getPassword() != key) {
                client->sendMessage(":server 475 " + client->getNickName() + " " + channelName + " :Cannot join channel (+k)");
                continue;
            }
        }

        // Join the channel
        if (!client->isInChannel(channel)) {
            channel->addClient(client);
            client->joinChannel(channel);

            // Send join notification to all channel members
            std::string joinMsg = ":" + client->getNickName() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN " + channelName;
            std::vector<Client*> clients = channel->getClients();
            for (size_t i = 0; i < clients.size(); ++i)
                clients[i]->sendMessage(joinMsg);

            // Send channel topic
            if (!channel->getTopic().empty())
                client->sendMessage(":server 332 " + client->getNickName() + " " + channelName + " :" + channel->getTopic());

            // Send names list
            std::string namesList;
            for (size_t i = 0; i < clients.size(); ++i) {
                if (channel->isOperator(clients[i]))
                    namesList += "@";
                namesList += clients[i]->getNickName() + " ";
            }
            client->sendMessage(":server 353 " + client->getNickName() + " = " + channelName + " :" + namesList);
            client->sendMessage(":server 366 " + client->getNickName() + " " + channelName + " :End of /NAMES list.");
        }
    }
}