#ifndef SERIALPORTIO_H
#define SERIALPORTIO_H
#include "iodevice.h"
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

class SerialPortIo : public IODevice
{
public:
    SerialPortIo(boost::asio::io_service &ios, std::string port, int baud);

    void read();
    void write(const std::vector<uint8_t> data);

private:
    std::vector<uint8_t> _buf;
    boost::asio::serial_port _port;
};

#endif // SERIALPORTIO_H
