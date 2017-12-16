#include "iodevice.h"

IODevice::IODevice()
{

}

void IODevice::listen(IOClientPtr c)
{
    std::unique_lock<std::mutex> l(_lock);
    c->setIO(this);
    _clients.push_back(c);
}

void IODevice::onData(const std::vector<uint8_t> & d)
{
    std::unique_lock<std::mutex> l(_lock);
    for (int i=0; i<_clients.size();++i){
        _clients[i]->onGotData(d);
    }
}

IOClient::IOClient():_io(nullptr)
{
}

void IOClient::write(const std::vector<uint8_t> buf)
{
    if (_io){
        _io->write(buf);
    }
}

void IOClient::setIO(IODevice * io)
{
    _io = io;
}
