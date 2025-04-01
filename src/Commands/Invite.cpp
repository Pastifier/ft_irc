#include "Commands.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channels.hpp"

void Invite::execute(Server *server, Client *client, const std::string& params) {
    if (!client->isRegistered()) {
        client->sendMessage(":server 451 * :You have not registered");
        return;
    }

    std::istringstream iss(params);
    std::string nickname, channelName;
    
    if (!(iss >> nickname >> channelName)) {
        client->sendMessage(":server 461 " + client->getNickName() + " INVITE :Not enough parameters");
        return;
    }

    Client* targetClient = server->findClientByNickname(nickname);
    if (!targetClient) {
        client->sendMessage(":server 401 " + client->getNickName() + " " + nickname + " :No such nick/channel");
        return;
    }

    Channels* channel = server->findChannels(channelName);
    if (!channel) {
        client->sendMessage(":server 403 " + client->getNickName() + " " + channelName + " :No such channel");
        return;
    }

    if (!channel->hasClient(client)) {
        client->sendMessage(":server 442 " + client->getNickName() + " " + channelName + " :You're not on that channel");
        return;
    }

    if (channel->hasClient(targetClient)) {
        client->sendMessage(":server 443 " + client->getNickName() + " " + nickname + " " + channelName + " :is already on channel");
        return;
    }

    if (channel->isInviteOnly() && !channel->isOperator(client)) {
        client->sendMessage(":server 482 " + client->getNickName() + " " + channelName + " :You're not channel operator");
        return;
    }

    // Send invite notification to the target client
    std::string inviteMsg = ":" + client->getNickName() + "!" + client->getUsername() + "@" + 
                           client->getHostname() + " INVITE " + nickname + " :" + channelName;
    targetClient->sendMessage(inviteMsg);

    // Send confirmation to the inviting client
    client->sendMessage(":server 341 " + client->getNickName() + " " + nickname + " " + channelName);
}