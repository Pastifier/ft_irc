#pragma once

#include <sys/poll.h>
#include <sys/signal.h>
#include <vector>
#include <map>
#include <poll.h>
#include <algorithm>
#include "Socket.hpp"
#include "Client.hpp"
#include "Channels.hpp"
#include "Commands.hpp"

#include "printing.hpp"

class IrcBot;

class Server {
public:
    Server(int port, const std::string& password = "");
    ~Server();
    
    void initialize();
    void run();
    
    void handleNewConnection();
    void handleClientData(size_t clientIndex);
    void registerClient(Client* client);
    void removeClient(Client* client);
    Client* findClientByNickname(const std::string& nickname);
    
    Channels* createChannels(const std::string& name);
    Channels* findChannels(const std::string& name) const;
    
    void registerCommands();
    void executeCommand(Client* client, const std::string& commandName,
                        const std::string& parameters);
	void processCommand(Client *client, const std::string& line);
    
    bool isAuthenticated(Client* client) const;
    bool isValidPassword(const std::string& password) const;

    void broadcastMessage(const std::string& message);
	bool isNicknameAvailable(const std::string& nickname) const;
	void sendToChannel(Channels *channel, const std::string& message, Client *exclude);
    
    void performScheduledTasks();
    
    const std::string& getName() const;
    const std::string& getVersion() const;
    const std::string& getPassword() const;
    const std::vector<Client*>& getClients() const;
    const std::map<std::string, Channels*>& getChannelss() const;

	/**
	 * @brief Bot related method
	 */
	IrcBot *findBot(const std::string& botName);
	IrcBot *createBot(const std::string& botName, const std::string& masterNick);

private:
    Socket* _serverSocket;
    std::vector<Client*> _clients;
    std::map<std::string, Channels*> _Channelss; // Active Channelss
    std::map<std::string, Commands*> _commands;

    struct pollfd* _pollfds;
    size_t _pollfdCount;
    size_t _pollfdCapacity;

    bool _running;
    std::string _password;
    std::string _name;
    std::string _version; // just tryna look official (>w<)~

    Server(Server const& other);
    Server& operator=(Server const& rhs);

    void _preparePollArray();
    void _resizePollArrayIfNeeded(size_t neededSize);
};
