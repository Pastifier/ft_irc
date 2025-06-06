#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <Client.hpp>
#include <algorithm>

class Client;

class Channels {
private:
	std::string _name;
	std::string _topic;
	std::string _password;
	unsigned int _userLimit; //Maximum number of clients allowed in the channel.
	bool _invite_only; //Can join the channel only if invitation is sent.
	bool _topicRestricted; //Who can change the topic
	// bool _has_topic;
	std::vector<Client *> _clients;
	std::vector<Client *> _operators;
	std::vector<Client *> _invitedUsers;
public:
	Channels(const std::string& name);
	~Channels();

	/**
	 * @brief Methods to handle invited users
	 */
	void addInvited(Client *client);
	void removeInvited(Client *client);
	bool isInvited(Client *client) const;
	std::vector<Client *> getInvitedUsers() const;

	/**
	 * @brief Getters/Setters
	 */
	const std::string& getName() const;
	void setTopic(const std::string& topic);
	const std::string& getTopic() const;
	void setPassword(const std::string& password);
	const std::string& getPassword() const;
	void setUserLimit(unsigned int limit);
	unsigned int getUserLimit() const;
	void setInviteOnly(bool inviteOnly);
	bool isInviteOnly() const;
	void setTopicRestricted(bool restricted);
	bool isTopicRestricted() const;

	/**
	 * @brief Client Operations
	 */
	void addClient(Client *client);
	void removeClient(Client *client);
	bool hasClient(Client *client) const;
	void addOperator(Client *client);
	void removeOperator(Client *client);
	bool isOperator(Client *client) const;

	/**
	 * @brief Channel Operations
	 */
	std::vector<Client *> getClients() const;
	std::vector<Client *> getOperators() const;
	size_t getClientCount() const;
};