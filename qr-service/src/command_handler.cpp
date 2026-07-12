// Header file
#include "command_handler.hpp"

// External libraries
#include <ctime>

CommandHandler::CommandHandler(QRReader& qrReader)
    : qrReader_(qrReader)
{
}

std::string CommandHandler::handle(const std::string& command){
    /* Handle a command and return a JSON response */
    if (command == "INIT"){
        if (qrReader_.init()){
            return R"({"status":"OK","type":"init"})";
        }
        return R"({"status":"ERROR","type":"init"})";
    }

    if (command == "PING"){
        if (qrReader_.ping()){
            return R"({"status":"OK","type":"pong"})";
        }
        return R"({"status":"ERROR","type":"ping"})";
    }

    if (command == "START"){
        std::string code = qrReader_.start(5);

        std::time_t timestamp = std::time(nullptr);

        return
            "{\"status\":\"OK\","
            "\"type\":\"start\","
            "\"qr-data\":{"
                "\"code\":\"" + code + "\","
                "\"ts\":" + std::to_string(timestamp) +
            "}}";
    }

    if (command == "STOP"){
        qrReader_.stop();
        return R"({"status":"OK","type":"stop"})";
    }
    return R"({"status":"ERROR","message":"Unknown command"})";
}