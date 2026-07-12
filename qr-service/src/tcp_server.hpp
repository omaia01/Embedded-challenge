#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

// External librarie
#include <string>

class TCPServer
{
public:
    explicit TCPServer(int port);

    void start();

private:
    std::string cleanCommand(const std::string& command);

    int port_;
    int serverSocket_;
};

#endif