// Header file
#include "tcp_server.hpp"

// External libraries
#include <iostream>

int main(){
    std::cout << "[INFO] Starting QR Service" << std::endl;

    TCPServer server(5000);

    server.start();

    return 0;
}