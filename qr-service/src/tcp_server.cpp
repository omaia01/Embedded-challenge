// Header files
#include "tcp_server.hpp"
#include "command_handler.hpp"

// Socket
#include <arpa/inet.h>
#include <unistd.h>

// Strings
#include <cstring>
#include <iostream>
#include <algorithm>


TCPServer::TCPServer(int port)
    /* Constructor sets initial state */
    : port_(port),
      serverSocket_(-1)
{
}


std::string TCPServer::cleanCommand(const std::string& command){
    /* Clean the command string by removing newline and carriage return characters */
    std::string cleaned = command;

    cleaned.erase(
        std::remove(cleaned.begin(), cleaned.end(), '\n'),
        cleaned.end());

    cleaned.erase(
        std::remove(cleaned.begin(), cleaned.end(), '\r'),
        cleaned.end());
    return cleaned;
}


void TCPServer::start(){
    /* Start the TCP server */
    // Socket creation
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket_ < 0){
        std::cerr << "Failed to create socket\n";
        return;
    }
    std::cout << "Socket created successfully!\n";

    // Adress config
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind
    int opt = 1;
    setsockopt(serverSocket_,
            SOL_SOCKET,
            SO_REUSEADDR,
            &opt,
            sizeof(opt));
    
    if (bind(serverSocket_, reinterpret_cast<sockaddr*>(&serverAddress),
         sizeof(serverAddress)) < 0){
        perror("bind");
        close(serverSocket_);
        return;
    }
    std::cout << "Socket bound to port "
            << port_ << std::endl;
    
    // Listen
    if (listen(serverSocket_, 5) < 0){
        std::cerr << "Failed to listen on socket\n";
        close(serverSocket_);
        return;
    }
    std::cout << "Listening on port " << port_
            << "..." << std::endl;

    QRReader qrReader;
    CommandHandler handler(qrReader);

    // Accept
    while (true){
        std::cout << "Waiting for client..." << std::endl;

        int clientSocket = accept(serverSocket_, nullptr, nullptr);

        if (clientSocket < 0){
            std::cerr << "Failed to accept client\n";
            continue;
        }

        std::cout << "Client connected!" << std::endl;

        while (true){
            char buffer[1024] = {0};

            int bytesReceived =
                recv(clientSocket,
                    buffer,
                    sizeof(buffer)-1,
                    0);

            if (bytesReceived <= 0){
                std::cout << "Client disconnected\n";
                break;
            }

            buffer[bytesReceived] = '\0';

            std::string command = cleanCommand(buffer);

            std::cout << "Command received: "
                    << command
                    << std::endl;

            std::string response = handler.handle(command);
            response += "\n";
            send(clientSocket, response.c_str(),
                response.length(), 0);

            std::cout << "Sent: "
                    << response
                    << std::endl;
        }
        close(clientSocket);
    }
    close(serverSocket_);
}