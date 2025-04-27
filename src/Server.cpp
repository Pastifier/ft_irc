#include "Server.hpp"
#include "IrcBot.hpp"

Server *g_server = NULL;

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

void Server::shutdown() {
	_running = false;
}

void Server::signal_handler(int signum) {
	(void)signum;
	if (g_server)
		g_server->shutdown();
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
	_commands["BOT"] = new Bot();
	_commands["CREATEBOT"] = new CreateBot();
	_commands["DCC"] = new Dcc();
	_commands["WHOIS"] = new Whois();
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
        int clientFd = _clients[i]->getSocket();
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
		if (!_running)
			break;

        if (activity < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            continue;
        }

        if (_pollfds[0].revents & POLLIN) {
            handleNewConnection();
        }

        for (size_t i = 1; i < _pollfdCount; i++) {
            if (_pollfds[i].revents & POLLIN) {
				//Find the client based on the socket fd.
                int clientFd = _pollfds[i].fd;
                size_t clientIndex = 0;
				bool clientfound = false;
                //Search for the client with this socket
				for (size_t j = 0; j < _clients.size(); j++) {
					if (_clients[j]->getSocket() == clientFd) {
						clientIndex = j;
						clientfound = true;
						break;
					}
				}
				if (clientfound)
					handleClientData(clientIndex);
				else {
					std::cerr << "Warning: No client found for socket " << clientFd << std::endl;
					//close the socket and mark it as invalid in the poll array.
					close(clientFd);
					_pollfds[i].fd = -1;
				}
            }
        }
    }
}

void Server::handleNewConnection() {
    try {
        Socket* clientSocket = _serverSocket->accept();
        if (clientSocket != NULL) {
            std::cout << "New client connected from " << clientSocket->getIpAddress() 
                      << ":" << clientSocket->getPort() << std::endl;
			int clientfd = clientSocket->getFd();
			std::string clientIp = clientSocket->getIpAddress();
            Client *client = new Client(clientfd);
			client->setHostname(clientIp);
			clientSocket->setInvalid();
			delete clientSocket;
			registerClient(client);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error accepting new connection: " << e.what() << std::endl;
    }
}

void Server::executeCommand(Client *client, const std::string& command, const std::string& params) {
	std::map<std::string, Commands*>::iterator it = _commands.find(command);
	if (it == _commands.end()) {
		std::string response = ":server 421 " + (client->isRegistered() ? client->getNickName() : "*") +
			" " + command + " :Unknown command\r\n";
		client->sendMessage(response);
		return;
	}
	if (!client->isAuthenticated() && command != "PASS" && command != "NICK" && command != "USER" && command != "QUIT") {
		std::string response = ":server 464 " + (client->isRegistered() ? client->getNickName(): "*") +
			" :Password required\r\n";
		client->sendMessage(response);
		return;
	}
	if (client->getNickName() == "*" && command != "PASS" && command != "NICK" && command != "USER" && command != "QUIT" && command != "HELP") {
		std::string response = ":server 451 * :You have not registered. Type HELP to learn how to register\r\n";
		client->sendMessage(response);
		return;
	}
	try {
		it->second->execute(this, client, params);
	} catch (const std::exception& e) {
		std::cerr << "Error executing command " << command << ":" << e.what() << std::endl;
		std::string response = ":server 500 " + (client->isRegistered() ? client->getNickName() : "*") +
			" :Internal server error\r\n";
		client->sendMessage(response);
	}
}

void Server::processCommand(Client *client, const std::string& line) {
	if (line.empty())
		return;
	std::string cleanline = line;
	if (!cleanline.empty() && cleanline[cleanline.size() - 1] == '\n')
		cleanline.erase(cleanline.size() - 1);
	if (!cleanline.empty() && cleanline[cleanline.size() - 1] == '\r')
		cleanline.erase(cleanline.size() - 1);
	std::string cmd;
	std::string params;
	size_t pos = cleanline.find(' ');
	if (pos == std::string::npos) {
		//No space found then only command with no parameters
		cmd = cleanline;
		params = "";
	} else {
		//Extract command and parameters
		cmd = cleanline.substr(0, pos);
		while (pos < cleanline.size() && cleanline[pos] == ' ')
			pos++;
		if (pos < cleanline.size())
			params = cleanline.substr(pos);
		else
			params = "";
	}
	//Convert command to uppercase for case-insensitive matching
	for (size_t i = 0; i < cmd.size(); i++)
		cmd[i] = toupper(cmd[i]);
	executeCommand(client, cmd, params);
}

void Server::handleClientData(size_t clientIndex)
{
	Client *client = _clients[clientIndex];
	if (!client) {
		ERROR("Client not found for socket");
		return;
	}
	int clientSocket = client->getSocket();
	if (clientSocket < 0)
		return; //skip processing for virtual client (Bot).
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead == 0) {
		removeClient(client);
		return;
	}
	else if (bytesRead < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
		//EAGAIN indicates that the resource is temporarily unavailable
		//EWOULDBLOCK indicates the operation cannot proceed at the moment because it would block
			perror("recv failed");
			removeClient(client);
		}
		return;
	}
	buffer[bytesRead] = '\0';
	client->appendToBuffer(buffer);
	int maxlines = 100;
	int lineCount = 0;
	while (client->hasCompleteLine() && lineCount < maxlines) {
		std::string line =  client->getLine();
		processCommand(client, line);
		lineCount++;
	}
	if (lineCount >= maxlines) {
		ERROR("Error: Reading the line");
	}
}

bool Server::isNicknameAvailable(const std::string& nickname) const {
	for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if ((*it)->getNickName() == nickname)
		return false;
	}
	return true;
}

bool Server::isAuthenticated(Client *client) const {
	if (!client)
		return false;
	if (_password.empty())
		return true;
	return client->isAuthenticated();
}

void Server::sendToChannel(Channels *channel, const std::string& message, Client *exclude) {
	std::vector<Client *> clients = channel->getClients();
	for (size_t i = 0; i < clients.size(); ++i) {
		if (clients[i] != exclude) {
			clients[i]->sendMessage(message);
		}
	}
}

void Server::removeClient(Client *client) {
	if (!client)
		return;
	std::vector<Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it) {
		if (*it == client)
			break;
	}
	if (it == _clients.end())
		return;
	// Remove client from all channels they've joined
	std::vector<Channels *> clientChannels = client->getChannels();
	for (size_t i = 0; i < clientChannels.size(); ++i) {
		Channels *channel = clientChannels[i];
		channel->removeClient(client);

		// If channel is empty after client removal, delete it as well
		if (channel->getClientCount() == 0) {
			std::map<std::string, Channels *>::iterator channelIt = _Channelss.find(channel->getName());
			if (channelIt != _Channelss.end()) {
				delete channelIt->second;
				_Channelss.erase(channelIt);
			}
		}
	}
	// Log before deleting the client
	std::cout << "Client disconnected: " << client->getNickName()
	          << " (" << client->getHostname() << ")" << std::endl;
	// Remove from _clients and cleanup
	_clients.erase(it);
	_preparePollArray();
	delete client;
}

void Server::registerClient(Client *client) {
	if (!client)
		return;
	_clients.push_back(client);
	_resizePollArrayIfNeeded(_pollfdCount + 1);
	_preparePollArray();
	std::string welcomeMessage = ":" + _name + " NOTICE :Welcome to " + _name + " v" + _version + "\r\n";
	client->sendMessage(welcomeMessage);
	std::cout << "New client registered from " << client->getHostname() << std::endl;
}

Channels *Server::createChannels(const std::string& name) {
	if (findChannels(name) != NULL)
		return findChannels(name);
	Channels *newChannel = new Channels(name);
	_Channelss[name] = newChannel;
	std::cout << "New channel created: " << name << std::endl;
	return newChannel;
}

Channels *Server::findChannels(const std::string& name) const {
	std::map<std::string, Channels*>::const_iterator it = _Channelss.find(name);
	if (it != _Channelss.end())
		return it->second;
	return NULL;
}

Client *Server::findClientByNickname(const std::string& nickname) {
	std::vector<Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it) {
		if ((*it)->getNickName() == nickname)
			return *it;
	}
	return NULL;
}

void Server::broadcastMessage(const std::string& message) {
	for (std::vector<Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (*it)
			(*it)->sendMessage(message);
	}
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

/**
 * @brief Bot related method expansion
 */

IrcBot *Server::findBot(const std::string& botName) {
	std::vector<Client*>::const_iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it) {
		IrcBot *bot = dynamic_cast<IrcBot *>(*it);
		if (bot != NULL && bot->getBotName() == botName)
			return bot;
	}
	return NULL;
}

IrcBot *Server::createBot(const std::string &botName, const std::string &masterNick) {
	IrcBot *bot = new IrcBot(-1, botName); //using -1 to indicate this is a virtual client so we dont have to create another socket.
	bot->setNickName(botName);
	bot->setUsername(botName);
	bot->setHostname("localhost");
	bot->setRealname("IRC Bot");
	bot->setMasterNickname(masterNick);

	//Register the client
	_clients.push_back(bot);
	return bot;
}