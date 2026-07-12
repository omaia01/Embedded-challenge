#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

// External librarie
#include <string>

// Header file
#include "qr_reader.hpp"

class CommandHandler
{
public:
    CommandHandler(QRReader& qrReader);

    std::string handle(const std::string& command);

private:
    QRReader& qrReader_;
};

#endif