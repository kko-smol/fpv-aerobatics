#ifndef UDPLISTENIO_H
#define UDPLISTENIO_H

#include "iodevice.h"
#include <boost/asio.hpp>


class UdpListenIo : public IODevice
{
public:
    UdpListenIo(boost::asio::io_service &ios, int port);

    void read();
    void write(const std::vector<uint8_t> data){} //cannot write

private:

    std::vector<uint8_t> _buf;

    boost::asio::ip::udp::endpoint _ep;
    boost::asio::ip::udp::socket _socket;
};

#endif // UDPLISTENIO_H
