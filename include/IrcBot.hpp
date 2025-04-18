#pragma once

#include "Client.hpp"
#include "Server.hpp"

class IrcBot: public Client {
private:
	std::string _botName;
	std::string _masterNickName;
	std::map<std::string, std::string> _commands;

	/**
	 * @brief Random response generator.
	 */
	std::vector<std::string> _greetings;
	std::vector<std::string> _jokes;

	bool _isActive;
	time_t _startTime;
public:
	IrcBot(int socket, const std::string& botName);
	void initializeCommands();
	void initializeResponses();
	void processMessage(Server *server, const std::string& channel, const std::string& sender, const std::string& message);
	void sendMsg(Server *server, const std::string& channel, const std::string& message);
	void sendCommandHelp(Server *server, const std::string& channel);
	void sendUpTime(Server *server, const std::string& channel);
	void sendRandomNumber(Server *server, const std::string& channel);
	void sendJoke(Server *server, const std::string& channel);
	void sendPong(Server *server, const std::string& channel);
	void sendRandomGreeting(Server *server, const std::string& channel);

	void setMasterNickname(const std::string& nickname);
	const std::string& getBotName() const;
};