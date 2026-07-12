// Header file
#include "logger.hpp"

// External librarie
#include <iostream>

void Logger::info(const std::string& message){
    /* Log an informational message */
    std::cout << "[INFO] " << message << std::endl;
}