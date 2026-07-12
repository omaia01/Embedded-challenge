#ifndef QR_READER_HPP
#define QR_READER_HPP

// External libraries
#include <string>
#include <atomic>


class QRReader
{
public:
    QRReader();
    ~QRReader();
    bool init();

    bool ping();

    std::string start(int timeoutSeconds);

    void stop();

private:
    bool initialized_;
    std::atomic<bool> stopRequested_;

    int serialFd_;

    std::string serialPort_;

    int baudRate_;
};

#endif