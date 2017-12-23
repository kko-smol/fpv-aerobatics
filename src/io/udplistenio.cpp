#include "udplistenio.h"


UdpListenIo::UdpListenIo(boost::asio::io_service &ios, int port):IODevice(),_ep(boost::asio::ip::udp::v4(),port),
    _socket(ios,_ep)
{
    _buf.resize(1024);
    read();
}

void UdpListenIo::read()
{
    _socket.async_receive(boost::asio::buffer(_buf.data(),_buf.size()),[this](
                          const boost::system::error_code& error, // Result of operation.
                          std::size_t bytes_transferred           // Number of bytes received.
                          ){
        std::vector<uint8_t> r;
        r.resize(bytes_transferred);
        memcpy(r.data(),_buf.data(),bytes_transferred);
        onData(r);
        read();
    });
}
