#pragma once

#include <string>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h> // for close(2)
#include <sys/socket.h>
#include <sys/types.h> // for `socklen_t`
#include <arpa/inet.h> // for `inet_ntoa(3)`
#include <netinet/in.h> // for `struct sockaddr_in`
#include <netdb.h> // just in case, for now
#include <stdexcept>

#define __FT_IRC_ERRNO_QUERY std::string(strerror(errno))

class Socket
{
public:
    Socket(int domain_, int type_, int protocol_) // (AF_INET, SOCK_STREAM, 0) for TCP/IPV4
        : _fd(-1), _isServer(false)
    {
        std::memset(&_address, 0, sizeof(_address));

        _fd = socket(domain_, type_, protocol_);
        if (_fd < 0)
            throw std::runtime_error("Socket creation failed: " + __FT_IRC_ERRNO_QUERY);

        int reuse = 1;
        if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
            throw std::runtime_error("Failed to set socket options: " + __FT_IRC_ERRNO_QUERY);
    }

    Socket(int fd_, struct sockaddr_in address_) // Create metadata for client from accept(2)
        : _fd(fd_), _address(address_), _isServer(false)
    {
        if (_fd < 0)
            throw std::runtime_error("Invalid socket descriptor.");
    }

    ~Socket() {
        if (_fd >= 0)
            close(_fd);
    }

    void bind(int port) {
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons(port);

        if (::bind(_fd, (struct sockaddr*)&_address, sizeof(_address)) < 0)
            throw std::runtime_error("Socket bind failed: " + __FT_IRC_ERRNO_QUERY);
        _isServer = true;
    }

    void listen(int backlog) { // (backlog = 10) def
        if (!_isServer)
            throw std::runtime_error("Cannot listen on a client socket!");
        
        if (::listen(_fd, backlog) < 0)
            throw std::runtime_error("Listen failed: " + __FT_IRC_ERRNO_QUERY);
    }

private:
    int _fd;
    struct sockaddr_in _address;
    bool _isServer;

    Socket(Socket const& other);
    Socket& operator=(Socket const& rhs);
};