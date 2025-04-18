#include <iostream>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include "Client.hpp"
#include "Channels.hpp"

Client::Client(int socket)
	: _socket(socket), _nickname("*"), _username(""), _hostname("localhost"), _realname(""),
	_authenticated(false), _registered(false), _buffer(""), _isOperator(false) {}

Client::~Client() {
	if (_socket >= 0) {
		close(_socket);
		_socket = -1;
	}
}

void Client::setNickName(const std::string& nickname) {
	_nickname = nickname;
}

const std::string& Client::getNickName() const {
	return _nickname;
}

void Client::setUsername(const std::string& username) {
	_username = username;
}

const std::string& Client::getUsername() const {
	return _username;
}

void Client::setRealname(const std::string& realname) {
	_realname = realname;
}

const std::string& Client::getRealname() const {
	return _realname;
}

void Client::setHostname(const std::string& hostname) {
	_hostname = hostname;
}

const std::string& Client::getHostname() const {
	return _hostname;
}

void Client::setAuthenticated(bool auth) {
	_authenticated = auth;
}

bool Client::isAuthenticated() const {
	return _authenticated;
}

void Client::setRegistered(bool reg) {
	_registered = reg;
}

bool Client::isRegistered() const {
	return _registered;
}

void Client::setOperator(bool op) {
	_isOperator = op;
}

bool Client::isOperator() const {
	return _isOperator;
}

int Client::getSocket() const {
	return _socket;
}

void Client::appendToBuffer(const std::string& data) {
	_buffer += data;
}

/**
 * @brief This function is designed to handle newline delimiters used
 * in network protocols, specifically IRC (which uses \r\n as the end-of-line marker).
 * @return std::string 
 */
std::string Client::getLine() {
	std::string line;
	size_t pos = _buffer.find("\r\n"); //Find eol maker (CR+LF in IRC protocol)
	if (pos != std::string::npos) {
		line = _buffer.substr(0, pos); //Extract the line
		_buffer = _buffer.substr(pos + 2); //Remove the line from buffer
	} else {
		pos = _buffer.find("\n");
		if (pos != std::string::npos) {
			line = _buffer.substr(0, pos);
			_buffer = _buffer.substr(pos + 1);
		} else {
			// No complete line found, return empty string
			return "";
		}
	}
	
	// Trim any trailing carriage returns
	if (!line.empty() && line[line.length() - 1] == '\r') {
		line = line.substr(0, line.length() - 1);
	}
	
	return line;
}

/**
 * @brief This function checks whether the _buffer contains a complete line by searching for newline 
 * delimiters ("\r\n" or "\n").
 * @return true 
 * @return false 
 */
bool Client::hasCompleteLine() const {
	return _buffer.find("\r\n") != std::string::npos || _buffer.find("\n") != std::string::npos;
}

/**
 * @brief This function sends a message to a connected client over a network connection. It ensures that 
 * the message is properly formatted with the appropriate newline characters (\r\n), and it handles errors
 * if the message fails to send or if not all bytes were sent.
 * @param message 
 */
void Client::sendMessage(const std::string& message) {
	if (_socket < 0)
		return; //Silently return for virtual clients.
	std::string formattedMessage = message;
	if (formattedMessage.length() < 2 ||
		formattedMessage.substr(formattedMessage.length() - 2) != "\r\n") {
		formattedMessage += "\r\n";
	}
	ssize_t byteSent = send(_socket, formattedMessage.c_str(), formattedMessage.length(), 0);
	if (byteSent < 0) {
		ERROR("Error: Sending message to client");
		ERROR(strerror(errno));
	} else if (static_cast<size_t>(byteSent) < formattedMessage.length()) {
		ERROR("Warning: Not all bytes sent to client");
	}
}

/**
 * @brief If the given channel is not available in the vector of channels pointer add it on the container
 * @param channel 
 */
void Client::joinChannel(Channels *channel) {
	if (std::find(_channels.begin(), _channels.end(), channel) == _channels.end()) {
		_channels.push_back(channel);
	}
}

/**
 * @brief leave channel
 * @param channel 
 */

void Client::leaveChannel(Channels *channel) {
	if (!channel)
		return;
	std::vector<Channels *>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); ++it) {
		if (*it == channel) {
			_channels.erase(it);
			if (channel->hasClient(this))
				channel->removeClient(this);
			break;
		}
	}
}

/**
 * @brief This function checks whether a Client is currently a member of a given Channel.
 * @param channel 
 * @return true 
 * @return false 
 */
bool Client::isInChannel(Channels *channel) const {
	return std::find(_channels.begin(), _channels.end(), channel) != _channels.end();
}

/**
 * @brief This function returns a copy of the list of channels the Client is currently a member of.
 * @return std::vector<Channels *> 
 */
std::vector<Channels *> Client::getChannels() const {
	return _channels;
}
