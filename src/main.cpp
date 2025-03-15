#include "Server.hpp"
#include <sstream>

int is_valid_port(std::string input) {
	std::istringstream iss(input);
	int portno;

	iss >> portno;
	if (iss.fail() || !iss.eof()) {
		std::cerr << "Error: invalid port number. Please make sure that the input is an integer in the range (1->65535)." << std::endl;
		return -1;
	} else if (portno < 1 || portno > 65535) {
		std::cerr << "Error: invalid port number. Please make sure that the input is an integer in the range (1->65535)." << std::endl;
		return -1;
	}
	return portno;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv port password" << std::endl;
		return 1;
	}
	std::string input(argv[1]);
	int port = is_valid_port(input);
	if (port == -1)
		return 1;
	return 0;
}
