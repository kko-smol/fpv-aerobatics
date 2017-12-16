#include "serialportio.h"
#include <boost/bind.hpp>
#include <iostream>
#include <linux/ioctl.h>
#include <linux/tty_flags.h>
#include <linux/serial.h>
SerialPortIo::SerialPortIo(boost::asio::io_service& ios, std::string port, int baud):_port(ios,port)
{
    _buf.resize(8);
    _port.set_option(boost::asio::serial_port_base::baud_rate(baud));
    _port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    boost::asio::basic_serial_port<boost::asio::serial_port_service>::native_type native = _port.native();
    struct serial_struct serial;
    ioctl(native, TIOCGSERIAL, &serial);
    serial.flags |= ASYNC_LOW_LATENCY; // (0x2000)
    ioctl(native, TIOCSSERIAL, &serial);

    if (!_port.is_open()){
        std::cerr << "Err open port";
    }

    read();
}

void SerialPortIo::read()
{

    _port.async_read_some(boost::asio::buffer(_buf.data(),_buf.size()),[this](boost::system::error_code e, std::size_t s){
        std::vector<uint8_t> buf;
        buf.resize(s);
        memcpy(buf.data(),_buf.data(),s);
        memset(_buf.data(),0,_buf.size());
        onData(buf);
        read();
    });
}

void SerialPortIo::write(const std::vector<uint8_t> data)
{
    boost::asio::async_write(_port,boost::asio::buffer(data.data(),data.size()),[](boost::system::error_code c, std::size_t){

    });
}
