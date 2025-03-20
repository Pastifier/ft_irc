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
    Client* findClientByNickname(const std::string& nickname) const;
    
    Channels* createChannels(const std::string& name);
    Channels* findChannels(const std::string& name) const;
    
    void registerCommands();
    bool executeCommand(Client* client, const std::string& commandName, 
                        const std::vector<std::string>& parameters);
    
    bool isAuthenticated(Client* client) const;
    bool isValidPassword(const std::string& password) const;

    void broadcastMessage(const std::string& message);
    
    void performScheduledTasks();
    
    const std::string& getName() const;
    const std::string& getVersion() const;
    const std::string& getPassword() const;
    const std::vector<Client*>& getClients() const;
    const std::map<std::string, Channels*>& getChannelss() const;

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

Server::Server(int port, const std::string& password)
    : _serverSocket(NULL),
      _pollfds(NULL),
      _pollfdCount(0),
      _pollfdCapacity(10),
      _running(false),
      _password(password),
      _name("ft_irc"),
      _version("1.0")
{
    try {
        _serverSocket = new Socket(AF_INET, SOCK_STREAM, 0);
        _serverSocket->bind(port);
        _serverSocket->listen(10);
        _serverSocket->setNonBlocking();

        _pollfds = new struct pollfd[_pollfdCapacity];
        _pollfdCount = 1;  // Start with one for the server socket

        _pollfds[0].fd = _serverSocket->getFd();
        _pollfds[0].events = POLLIN;

        PRINT("Server started on port " << port);
    } 
    catch (const std::exception& e) {
        ERROR("Server initialization error: " << e.what());
        if (_serverSocket) {
            delete _serverSocket;
            _serverSocket = NULL;
        }
        if (_pollfds) {
            delete[] _pollfds;
            _pollfds = NULL;
        }
        throw;
    }
}

Server::~Server() {
    for (size_t i = 0; i < _clients.size(); i++)
        delete _clients[i];
    _clients.clear();

    for (std::map<std::string, Channels*>::iterator it = _Channelss.begin(); 
         it != _Channelss.end(); ++it)
        delete it->second;
    _Channelss.clear();

    for (std::map<std::string, Commands*>::iterator it = _commands.begin(); 
         it != _commands.end(); ++it)
        delete it->second;
    _commands.clear();

    delete _serverSocket;
    delete[] _pollfds;
}

void Server::registerCommands() {
    _commands["PASS"] = new Pass();
    _commands["NICK"] = new Nick();
    _commands["USER"] = new User();
    _commands["PRIVMSG"] = new Privmsg();
    _commands["JOIN"] = new Join();
    _commands["PART"] = new Part();
    _commands["QUIT"] = new Quit();
    _commands["KICK"] = new Kick();
    _commands["INVITE"] = new Invite();
    _commands["TOPIC"] = new Topic();
    _commands["MODE"] = new Mode();
    _commands["NOTICE"] = new Notice();
    _commands["CAP"] = new Cap();
    _commands["HELP"] = new Help();
    _commands["PING"] = new Ping();
    _commands["OPER"] = new Oper();

}

void Server::initialize() {
    registerCommands();
    _running = true;
}

void Server::_preparePollArray() {
    _pollfds[0].fd = _serverSocket->getFd();
    _pollfds[0].events = POLLIN;
    _pollfds[0].revents = 0;

    _resizePollArrayIfNeeded(_clients.size() + 1);

    _pollfdCount = 1;  // Reset counter (server socket is always first)
    for (size_t i = 0; i < _clients.size(); i++) {
        int clientFd = -1;  // Will be set to the client's socket fd in the future
        if (clientFd != -1) {
            _pollfds[_pollfdCount].fd = clientFd;
            _pollfds[_pollfdCount].events = POLLIN;
            _pollfds[_pollfdCount].revents = 0;
            _pollfdCount++;
        }
    }
}

void Server::_resizePollArrayIfNeeded(size_t neededSize) {
    if (neededSize <= _pollfdCapacity)
        return;

    size_t newCapacity = _pollfdCapacity * 2;
    while (newCapacity < neededSize)
        newCapacity *= 2;
    
    struct pollfd* newArray = new struct pollfd[newCapacity];

    for (size_t i = 0; i < _pollfdCount; i++)
        newArray[i] = _pollfds[i];

    delete[] _pollfds;
    _pollfds = newArray;
    _pollfdCapacity = newCapacity;
}

void Server::run() {
    while (_running)
    {
        _preparePollArray();

        int activity = poll(_pollfds, _pollfdCount, -1);

        if (activity < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            continue;
        }

        if (_pollfds[0].revents & POLLIN) {
            handleNewConnection();
        }

        for (size_t i = 1; i < _pollfdCount; i++) {
            if (_pollfds[i].revents & POLLIN) {
                // Find client index based on the socket fd
                size_t clientIndex = i - 1;  // Simplification, will need to be updated
                handleClientData(clientIndex);
            }
        }
        
        // Perform any scheduled tasks (like bot actions)
        performScheduledTasks();
    }
}

void Server::handleNewConnection() {
    try {
        Socket* clientSocket = _serverSocket->accept();
        if (clientSocket != NULL) {

            // Client connection logic will go here

            std::cout << "New client connected from " << clientSocket->getIpAddress() 
                      << ":" << clientSocket->getPort() << std::endl;
                      
            // Placeholder for Client object creation
            // Client* client = new Client(clientSocket);
            // registerClient(client);
            
            // For now, just close the connection
            delete clientSocket;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error accepting new connection: " << e.what() << std::endl;
    }
}

void Server::handleClientData(size_t clientIndex)
{
    // This is a placeholder for client data handling
    // Will be implemented once we have the Client class
    if (clientIndex < _clients.size()) {
        // Client message handling will go here
    }
}

void Server::performScheduledTasks()
{
    // Placeholder for scheduled tasks like bot actions
}

bool Server::isValidPassword(const std::string& password) const {
    return password == _password;
}

const std::string& Server::getName() const {
    return _name;
}

const std::string& Server::getVersion() const {
    return _version;
}

const std::string& Server::getPassword() const {
    return _password;
}

const std::vector<Client*>& Server::getClients() const {
    return _clients;
}

const std::map<std::string, Channels*>& Server::getChannelss() const {
    return _Channelss;
}