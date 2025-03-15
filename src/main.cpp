#include "../include/Server.hpp"

int is_valid_port(std::string input) {
	try {
		size_t idx = 0;
		int number = stoi(input, &idx);
		if (idx != input.size())
			throw std::invalid_argument("Error: The value of the port is not integer.\n");
		if (number < 1 || number > 65535) {
			throw std::out_of_range("Error: The port number is out of range (1-65535).");
		}
		return number;
	} catch (const std::invalid_argument& e) {
		std::cerr << "Error: The value of the port is not integer." << std::endl;
	} catch (const std::out_of_range& e) {
		std::cout << "Error: The port number is out of range (1-65535)." << std::endl;
	}
	return -1;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv port password" << std::endl;
		return 1;
	}
	std::string input = argv[1];
	int port = is_valid_port(input);
	if (port == -1)
		return 1;
	return 0;
}
