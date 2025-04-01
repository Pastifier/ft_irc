#include "Commands.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channels.hpp"

void Topic::execute(Server *server, Client *client, const std::string& params) {
    if (!client->isRegistered()) {
        client->sendMessage(":server 451 * :You have not registered");
        return;
    }

    std::istringstream iss(params);
    std::string channelName;
    if (!(iss >> channelName)) {
        client->sendMessage(":server 461 " + client->getNickName() + " TOPIC :Not enough parameters");
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

    std::string topic;
    std::getline(iss >> std::ws, topic);

    if (topic.empty()) {
        // Query the current topic
        if (channel->getTopic().empty()) {
            client->sendMessage(":server 331 " + client->getNickName() + " " + channelName + " :No topic is set");
        } else {
            client->sendMessage(":server 332 " + client->getNickName() + " " + channelName + " :" + channel->getTopic());
        }
        return;
    }

    // Setting a new topic
    if (channel->isTopicRestricted() && !channel->isOperator(client)) {
        client->sendMessage(":server 482 " + client->getNickName() + " " + channelName + " :You're not channel operator");
        return;
    }

    // Remove the leading ':' if present
    if (topic[0] == ':')
        topic = topic.substr(1);

    channel->setTopic(topic);

    // Notify all channel members about the topic change
    std::string notification = ":" + client->getNickName() + "!" + client->getUsername() + "@" + 
                             client->getHostname() + " TOPIC " + channelName + " :" + topic;
    std::vector<Client*> clients = channel->getClients();
    for (size_t i = 0; i < clients.size(); ++i)
        clients[i]->sendMessage(notification);
}