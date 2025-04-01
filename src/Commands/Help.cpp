#include "Commands.hpp"
#include "Server.hpp"
#include "Client.hpp"

void Help::execute(Server *server, Client *client, const std::string& params) {
    (void)server; // Unused parameter
    (void)params; // Unused parameter

    client->sendMessage(":server 704 " + client->getNickName() + " :Available commands:");
    client->sendMessage(":server 705 " + client->getNickName() + " :PASS <password> - Register with server password");
    client->sendMessage(":server 705 " + client->getNickName() + " :NICK <nickname> - Set or change nickname");
    client->sendMessage(":server 705 " + client->getNickName() + " :USER <username> <hostname> <servername> :<realname> - Set user information");
    client->sendMessage(":server 705 " + client->getNickName() + " :JOIN <channel>{,<channel>} [<key>{,<key>}] - Join channel(s)");
    client->sendMessage(":server 705 " + client->getNickName() + " :PART <channel>{,<channel>} - Leave channel(s)");
    client->sendMessage(":server 705 " + client->getNickName() + " :PRIVMSG <target> :<message> - Send message to user/channel");
    client->sendMessage(":server 705 " + client->getNickName() + " :NOTICE <target> :<message> - Send notice to user/channel");
    client->sendMessage(":server 705 " + client->getNickName() + " :TOPIC <channel> [:<topic>] - View or set channel topic");
    client->sendMessage(":server 705 " + client->getNickName() + " :INVITE <nickname> <channel> - Invite user to channel");
    client->sendMessage(":server 705 " + client->getNickName() + " :KICK <channel> <user> [:<reason>] - Remove user from channel");
    client->sendMessage(":server 705 " + client->getNickName() + " :MODE <target> <modes> [<mode-params>] - Set user/channel modes");
    client->sendMessage(":server 705 " + client->getNickName() + " :OPER <name> <password> - Gain operator privileges");
    client->sendMessage(":server 705 " + client->getNickName() + " :QUIT [:<message>] - Disconnect from server");
    client->sendMessage(":server 705 " + client->getNickName() + " :PING <server> - Test connection to server");
    client->sendMessage(":server 705 " + client->getNickName() + " :BOT <command> - Interact with IRC bot");
    client->sendMessage(":server 706 " + client->getNickName() + " :End of /HELP");
}