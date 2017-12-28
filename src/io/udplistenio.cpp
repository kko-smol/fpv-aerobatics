#include "udplistenio.h"


UdpListenIo::UdpListenIo(boost::asio::io_service &ios, int port):IODevice(),_ep(boost::asio::ip::udp::v4(),port),
    _socket(ios)
{
    _socket.open(_ep.protocol());
    _socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    _socket.bind(_ep);
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

void UdpListenIo::write(const std::vector<uint8_t> data)
{
    _socket.send_to(boost::asio::buffer(data.data(),data.size()),
                          boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"),14550));

}
