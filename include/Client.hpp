#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "printing.hpp"
#include "Channels.hpp"

class Channels;

class Client {
private:
	int _socket;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _realname;
	bool _isOperator;
	bool _authenticated;
	bool _registered;
	std::string _buffer;
	std::vector<Channels *> _channels;
public:
	Client(int socket);
	~Client();

	/**
	 * @brief Getters/Setters
	 */
	void setNickName(const std::string& nickname);
	const std::string& getNickName() const;
	void setUsername(const std::string& username);
	const std::string& getUsername() const;
	void setRealname(const std::string& realname);
	const std::string& getRealname() const;
	void setHostname(const std::string& hostname);
	const std::string& getHostname() const;
	void setAuthenticated(bool auth);
	bool isAuthenticated() const;
	void setRegistered(bool reg);
	bool isRegistered() const;
	void setOperator(bool op);
	bool isOperator() const;
	int getSocket() const;

	/**
	 * @brief Buffer handling
	 */
	void appendToBuffer(const std::string& data);
	std::string getLine();
	bool hasCompleteLine() const;

	/**
	 * @brief Channel operations
	 */
	void joinChannel(Channels *channel);
	void leaveChannel(Channels *channel);
	bool isInChannel(Channels *channel) const;
	std::vector<Channels *> getChannels() const;

	/**
	 * @brief Message Handling
	 */
	void sendMassage(const std::string& message);
};