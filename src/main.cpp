
#include "Server.hpp"
#include <sstream>

int is_valid_port(std::string input) {
	std::istringstream iss(input);
	int portno;

	iss >> portno;
	if (iss.fail() || !iss.eof()) {
		ERROR("Error: invalid port number. Please make sure that the input is an integer in the range (1->65535).");
		return -1;
	} else if (portno < 1 || portno > 65535) {
		ERROR("Error: invalid port number. Please make sure that the input is an integer in the range (1->65535).");
		return -1;
	}
	return portno;
}

bool is_valid_password(const std::string& pass) {
	if (pass.size() == 0) {
		ERROR("Error: Password cannot be empty!");
		return false;
	}
	for (size_t i = 0; i < pass.size(); i++) {
        if (std::isspace(pass[i]) || pass[i] == '\t') {
			ERROR("Error: Password cannot contain space or tabs!");
            return false;
        }
	}
	return true;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: ./ircserv port password" << std::endl;
		return 1;
	}
	std::string input(argv[1]);
	std::string pass = argv[2];
	int port = is_valid_port(input);
	if (port == -1 || !is_valid_password(pass))
		return 1;
	try {
		Server server(port, pass);
		server.initialize();
		server.run();
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
