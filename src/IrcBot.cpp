#include "IrcBot.hpp"

IrcBot::IrcBot(int socket, const std::string& botName)
	: Client(socket), _botName(botName), _masterNickName(""), _isActive(true), _startTime(time(NULL)) {
	initializeCommands();
	initializeResponses();
}

void IrcBot::initializeCommands() {
	_commands["!help"] = "Display available Bot commands.";
	_commands["!uptime"] = "Show how long the Bot has been Running.";
	_commands["!random"] = "Generates a random number.";
	_commands["!joke"] = "Tells a random joke.";
	_commands["!ping"] = "Responds wihth Pong.";
}

void IrcBot::initializeResponses() {
	_greetings.clear();
	_greetings.push_back("Hello!");
	_greetings.push_back("Hi there!");
	_greetings.push_back("Greetings!");
	_greetings.push_back("What's up?");
	_greetings.push_back("Nice to meet you!");

	_jokes.clear();
	_jokes.push_back("Why C programmers have trouble dating? Because they don't like to deal with classes.");
	_jokes.push_back("Why did the C programmer break up with his girlfriend? Because she had too many issues with pointers.");
	_jokes.push_back("C programmers never die.. They just get dereferenced.");
	_jokes.push_back("How do Robots eat pizza? One byte at a time.");
	_jokes.push_back("What is the object oriented way to become wealthy? Through inheritance.");
}

void IrcBot::processMessage(Server *server, const std::string& channel, const std::string& sender, const std::string& message) {
	if (!_isActive) return;
	std::string trimmedMsg = message;
	trimmedMsg.erase(0, trimmedMsg.find_first_not_of(" "));
	trimmedMsg.erase(trimmedMsg.find_last_not_of(" ") + 1);
	if (trimmedMsg == "!help")
		sendCommandHelp(server, channel);
	else if (trimmedMsg == "!uptime")
		sendUpTime(server, channel);
	else if (trimmedMsg == "!random")
		sendRandomNumber(server, channel);
	else if (trimmedMsg == "!joke")
		sendJoke(server, channel);
	else if (trimmedMsg == "!ping")
		sendPong(server, channel);
	else if (trimmedMsg == "!deactivate" && sender == _masterNickName) {
		_isActive = false;
		sendMsg(server, channel, "Bot Deactivated!.");
	}
	else if (message.find(_botName) != std::string::npos) {
		sendRandomGreeting(server, channel);
	}
}

void IrcBot::sendMsg(Server *server, const std::string& channel, const std::string& message) {
	std::string fullMessage = ":" + this->getNickName() + "!" + this->getUsername() + "@" +
								this->getHostname() + " PRIVMSG " + channel + " :" + message + "\r\n";
	Channels *targetChannel = server->findChannels(channel);
	if (targetChannel)
		server->sendToChannel(targetChannel, fullMessage, NULL);
	else {
		Client *targetClient = server->findClientByNickname(channel);
		if (targetClient)
			targetClient->enqueueMessage(fullMessage);
		else {
			this->sendMsg(server, this->getNickName(), "401 " + this->getNickName() + " "
				+ channel + " :No such nick/channel");
		}
	}
}

void IrcBot::sendCommandHelp(Server *server, const std::string& channel) {
	sendMsg(server, channel, "Available commands:");
	for (std::map<std::string, std::string>::iterator it = _commands.begin(); it != _commands.end(); ++it) {
		sendMsg(server, channel, it->first + ": " + it->second);
	}
}

void IrcBot::sendUpTime(Server *server, const std::string& channel) {
	time_t currentTime = time(NULL);
	long uptime = currentTime - _startTime;
	std::ostringstream uptimeMsg;
	uptimeMsg << "I've been running for "
				<< uptime / 3600 << " hours, "
				<< (uptime % 3600) / 60 << " minutes, "
				<< uptime % 60 << " seconds.";
	sendMsg(server, channel, uptimeMsg.str());
}

void IrcBot::sendRandomNumber(Server *server, const std::string& channel) {
	int randomNum = rand() % 100 + 1; //Random no between 1-100
	std::ostringstream randomMsg;
	randomMsg << "Random number: " << randomNum;
	sendMsg(server, channel, randomMsg.str());
}

void IrcBot::sendJoke(Server *server, const std::string& channel) {
	if (!_jokes.empty()) {
		int index = rand() % _jokes.size();
		sendMsg(server, channel, _jokes[index]);
	}
}

void IrcBot::sendPong(Server *server, const std::string& channel) {
	sendMsg(server, channel, "Pong!");
}

void IrcBot::sendRandomGreeting(Server *server, const std::string& channel) {
	if (!_greetings.empty()) {
		int index = rand() % _greetings.size();
		sendMsg(server, channel, _greetings[index]);
	}
}

void IrcBot::setMasterNickname(const std::string& nickname) {
	_masterNickName = nickname;
}

const std::string& IrcBot::getBotName() const {
	return _botName;
}
