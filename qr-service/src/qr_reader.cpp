// Header file
#include "qr_reader.hpp"

// Time
#include <chrono>
#include <thread>

// Serial communication
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>    
#include <cstring>   
#include <iostream>


QRReader::QRReader()
/* Constructor sets initial state */
: initialized_(false),
  stopRequested_(false),
  serialFd_(-1),
  serialPort_("/tmp/ttyS1"),
  baudRate_(9600)
{
}


bool QRReader::init(){
    /* Initialize the serial port */
    if (serialFd_ >= 0){
        close(serialFd_);
        serialFd_ = -1;
    }
    initialized_ = false;
    const char* envPort = std::getenv("SERIAL_PORT");

    if (envPort != nullptr){
        serialPort_ = envPort;
    }
    const char* envBaud = std::getenv("SERIAL_BAUD");

    if (envBaud != nullptr){
        baudRate_ = std::stoi(envBaud);
    }
    serialFd_ = open(serialPort_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (serialFd_ < 0){
        std::cerr << "Failed to open serial port: " << serialPort_ << " errno=" 
          << errno << " (" << strerror(errno) << ")" << std::endl;
        return false;
    }
    termios tty{};

    if (tcgetattr(serialFd_, &tty) != 0){
        std::cerr << "Failed to configure serial port" << std::endl;
        close(serialFd_);
        serialFd_ = -1;
        return false;
    }
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag = 0;
    tty.c_iflag = 0;
    tty.c_oflag = 0;

    if (tcsetattr(serialFd_, TCSANOW, &tty) != 0){
        std::cerr << "Failed to configure serial port\n";
        close(serialFd_);
        serialFd_ = -1;
        return false;
    }
    initialized_ = true;
    return true;
}


bool QRReader::ping(){
    /* Health check */
    return initialized_ && serialFd_ >= 0;
}


std::string QRReader::start(int timeoutSeconds){
    /* Read a QR code */
    std::cout << "Waiting for QR..." << std::endl;

    if (!initialized_){
        return "NOT_INITIALIZED";
    }
    stopRequested_ = false;
    auto startTime = std::chrono::steady_clock::now();
    char buffer[256];

    while (!stopRequested_){
        int bytesRead = read(serialFd_, buffer, sizeof(buffer) - 1);

        if (bytesRead > 0){
            buffer[bytesRead] = '\0';
            return std::string(buffer);
        }
        auto elapsed =std::chrono::steady_clock::now() - startTime;

        if (std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()
            >= timeoutSeconds){
            // Return timeout if no data arrived within the given period
            return "TIMEOUT";
            std::cout << "Timeout waiting for QR" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return "STOPPED";
}


void QRReader::stop(){
    /* Request the serial read loop to stop */
    stopRequested_ = true;
}


QRReader::~QRReader(){
    /* Clean up the serial handle when the reader is destroyed */
    if (serialFd_ >= 0){
        close(serialFd_);
    }
}