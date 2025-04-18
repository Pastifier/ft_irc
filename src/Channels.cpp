#include "Channels.hpp"

Channels::Channels(const std::string& name) : _name(name)
, _topic(""), _password(""), _userLimit(0), _invite_only(false), _topicRestricted(true) {}

Channels::~Channels() {}

const std::string& Channels::getName() const {
	return _name;
}

void Channels::setTopic(const std::string& topic) {
	_topic = topic;
}

const std::string& Channels::getTopic() const {
	return _topic;
}

void Channels::setPassword(const std::string& password) {
	_password = password;
}

const std::string& Channels::getPassword() const {
	return _password;
}

void Channels::setUserLimit(unsigned int limit) {
	_userLimit = limit;
}

unsigned int Channels::getUserLimit() const {
	return _userLimit;
}

void Channels::setInviteOnly(bool inviteOnly) {
	_invite_only = inviteOnly;
}

bool Channels::isInviteOnly() const {
	return _invite_only;
}

void Channels::setTopicRestricted(bool restricted) {
	_topicRestricted = restricted;
}

bool Channels::isTopicRestricted() const {
	return _topicRestricted;
}

void Channels::addClient(Client *client) {
	if (std::find(_clients.begin(), _clients.end(), client) == _clients.end()) {
		_clients.push_back(client);
		client->joinChannel(this);
	}
}

void Channels::removeClient(Client *client) {
	std::vector<Client *>::iterator it = std::find(_clients.begin(), _clients.end(), client);
	if (it != _clients.end()) {
		_clients.erase(it);
		client->leaveChannel(this);
		removeOperator(client);
	}
}

bool Channels::hasClient(Client *client) const {
	return std::find(_clients.begin(), _clients.end(), client) != _clients.end();
}

void Channels::addOperator(Client *client) {
	if (!hasClient(client))
		return;
	if (std::find(_operators.begin(), _operators.end(), client) == _operators.end()) {
		_operators.push_back(client);
	}
}

void Channels::removeOperator(Client *client) {
	std::vector<Client *>::iterator it = std::find(_operators.begin(), _operators.end(), client);
	if (it != _operators.end()) {
		_operators.erase(it);
	}
}

bool Channels::isOperator(Client *client) const {
	return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

std::vector<Client *> Channels::getClients() const {
	return _clients;
}

size_t Channels::getClientCount() const {
	return _clients.size();
}

std::vector<Client *> Channels::getOperators() const {
	return _operators;
}

void Channels::addInvited(Client *client) {
	if (!isInvited(client)) {
		_invitedUsers.push_back(client);
	}
}

void Channels::removeInvited(Client *client) {
	for (std::vector<Client *>::iterator it = _invitedUsers.begin(); it != _invitedUsers.end(); ++it) {
		if (*it == client) {
			_invitedUsers.erase(it);
			break;
		}
	}
}

bool Channels::isInvited(Client *client) const {
	for (size_t i = 0; i < _invitedUsers.size(); ++i) {
		if (_invitedUsers[i] == client)
			return true;
	}
	return false;
}

std::vector<Client *> Channels::getInvitedUsers() const {
	return _invitedUsers;
}
