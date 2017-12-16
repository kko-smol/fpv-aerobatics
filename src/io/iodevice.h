#ifndef IODEVICE_H
#define IODEVICE_H

#include <mutex>
#include <memory>
#include <vector>
#include <array>

class IODevice;

class IOClient {
public:
    IOClient();
    virtual void onGotData(const std::vector<uint8_t>& buf) = 0;
    void write(const std::vector<uint8_t> buf);

    void setIO(IODevice*);
private:
    IODevice* _io;
};
typedef std::shared_ptr<IOClient> IOClientPtr;

class IODevice
{
public:
    IODevice();

    void listen(IOClientPtr c);
    void onData(const std::vector<uint8_t>& );

    virtual void write(const std::vector<uint8_t> buf) = 0;

private:
    std::mutex _lock;
    std::vector<IOClientPtr> _clients;
};

#endif // IODEVICE_H
