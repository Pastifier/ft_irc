#pragma once

#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "printing.hpp"
#include "Channels.hpp"

extern size_t g_enqueueCount;

class Channels;

class Client {
private:
	int _socket;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _realname;
	// std::string _outbuff;
	bool _authenticated;
	bool _registered;
	std::string _inbuff;
	bool _isOperator;
	std::vector<Channels *> _channels;
public:
	Client(int socket);
	virtual ~Client();

	/**
	 * @brief Getters/Setters
	 */
	void setNickName(const std::string& nickname);
	virtual const std::string& getNickName() const;
	void setUsername(const std::string& username);
	virtual const std::string& getUsername() const;
	void setRealname(const std::string& realname);
	const std::string& getRealname() const;
	void setHostname(const std::string& hostname);
	virtual const std::string& getHostname() const;
	void setAuthenticated(bool auth);
	bool isAuthenticated() const;
	void setRegistered(bool reg);
	bool isRegistered() const;
	void setOperator(bool op);
	bool isOperator() const;
	int getSocket() const;
	std::string getInBuffer() const { return _inbuff; }
	// std::string getOutBuffer() const { return _outbuff; }
	// void clearOutBuffer() { _outbuff.clear(); }
	void clearInBuffer() {_inbuff.clear(); }
	std::queue<std::string> outputMessages;

	/////
	void enqueueMessage(const std::string& message) {
		outputMessages.push(message + "\r\n");
		++g_enqueueCount;
	}

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
	// void enqueueMessage(const std::string& message);
	void sendMessage(const std::string& message);
};